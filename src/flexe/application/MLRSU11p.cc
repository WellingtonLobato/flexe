#include "MLRSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(MLRSU11p);

void MLRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0){
        self = getParentModule()->getIndex();
        cloudModuleName = par("cloudModuleName").stringValue();
        trainFlag = par("trainFlag").boolValue();
        epochs = par("epochs").intValue();
        batchSize = par("batchSize").intValue();
        modelName = par("modelName").stringValue();
        address = par("address").stringValue();

        selectIDMetric = registerSignal("selectID");
        roundMetric = registerSignal("round");
    }else if(stage == 1){
        //First Training
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        client->initialize_parameters(self, trainFlag, modelName, epochs, batchSize);
        //First Training
    }
}

MLRSU11p::~MLRSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(sendModelEvt);
    client->end("");
    delete client;
}

void MLRSU11p::onBSM(DemoSafetyMessage* bsm){
}

void MLRSU11p::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_CLOUD_EVT:{
        std::cout << self << " (RSU|onWSM) SEND_CLOUD_EVT " << simTime().dbl() << endl;

        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);

        for(int i=0; i < flexe_msg->getTensorsSizeArraySize(); i++){
            tensorSizeVector.push_back(flexe_msg->getTensorsSize(i));
        }

        for(int i=0; i < flexe_msg->getTensorsArraySize(); i++){
            tensorsBytes.push_back(flexe_msg->getTensors(i));
        }

        num_examples = flexe_msg->getNum_examples();
        modelStr = std::string(begin(tensorsBytes), end(tensorsBytes));
        modelsMap.insert(std::make_pair(flexe_msg->getSenderID(), std::make_tuple(modelStr, tensorSizeVector, num_examples)));

        // CLEAR MEMORY
        std::vector<char>().swap(tensorsBytes);
        tensorsBytes.clear();
        tensorsBytes.shrink_to_fit();

        std::vector<int>().swap(tensorSizeVector);
        tensorSizeVector.clear();
        tensorSizeVector.shrink_to_fit();

        modelStr.clear();
        // CLEAR MEMORY

        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl(), sendCloudEvt);
        }
        break;
    }
    default:{
        std::cout << self << " (RSU|onWSM) The message type was not detected. " << wsm->getKind() << endl;
        break;
    }
    }
}

void MLRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void MLRSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) SEND_CLOUD_EVT " << simTime().dbl() << endl;
        if(cloudModuleName.compare("NO_NAME") != 0){
            for(it = modelsMap.begin(); it != modelsMap.end(); ++it){
                ModelRequest requestModel;
                requestModel.set_idvehicle(it->first+1);
                requestModel.set_number_of_vehicles(modelsMap.size());
                requestModel.set_num_examples(std::get<2>(it->second));

                counter = 0;
                for(std::vector<int>::iterator jt = std::get<1>(it->second).begin(); jt != std::get<1>(it->second).end(); ++jt){
                    if(counter == 0){
                        strWeights = std::get<0>(it->second).substr(counter, *jt+counter);
                        requestModel.add_tensors(strWeights);
                        counter = counter + *jt - 1;
                    }else{
                        strWeights = std::get<0>(it->second).substr(counter+1, *jt+counter);
                        requestModel.add_tensors(strWeights);
                        counter = counter + *jt;
                    }
                }
                client->aggregate_fit(requestModel);
                client->aggregate_evaluate(self, trainFlag, modelName, batchSize);
                emit(selectIDMetric, it->first);
                scheduleAt(simTime().dbl()+uniform(0.0,0.01), sendModelEvt);
            }
            modelsMap.clear();
        }else{
            std::cout << self << " (RSU|handleSelfMsg) Cloud not defined " << simTime().dbl() << endl;
        }

        break;
    }
    case SEND_FED_MODEL_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) SEND_FED_MODEL_EVT " << simTime().dbl() << endl;
        FlexeMessage* flexe_msg = new FlexeMessage();
        DemoBaseApplLayer::populateWSM(flexe_msg);

        flexe_msg->setSenderID(self);
        flexe_msg->setKind(SEND_FED_MODEL_EVT);
        emit(roundMetric, (double)round);
        round++;
        DemoBaseApplLayer::sendDelayedDown(flexe_msg, uniform(0.0,0.001));
        break;
    }
    case CLOUD_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) CLOUD_EVT " << simTime().dbl() << endl;
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        DemoBaseApplLayer::populateWSM(bsm);
        bsm->setKind(SEND_FED_MODEL_EVT);
        DemoBaseApplLayer::sendDelayedDown(bsm, uniform(0.0,0.1));
        break;
    }
    default: {
        std::cout << self << " (RSU|handleSelfMsg) The message type was not detected. " << msg->getKind() << endl;
        break;
    }
    }
}
