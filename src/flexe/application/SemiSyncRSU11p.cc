#include "SemiSyncRSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(SemiSyncRSU11p);

SemiSyncRSU11p::~SemiSyncRSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(sendModelEvt);
    client->end("");
    delete client;
}


void SemiSyncRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0){
        self = getParentModule()->getIndex();
        epochs = par("epochs").intValue();
        batchSize = par("batchSize").intValue();

        trainFlag = par("trainFlag").boolValue();

        modelName = par("modelName").stringValue();
        cloudModuleName = par("cloudModuleName").stringValue();
        address = par("address").stringValue();

        roundDeadlineTime = par("roundDeadlineTime").doubleValue();

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

void SemiSyncRSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        break;
    }
    default:{
        std::cout << self << " (RSU|onBSM) The message type was not detected. " << bsm->getKind() << endl;
        break;
    }
    }
}

void SemiSyncRSU11p::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_CLOUD_EVT:{
        std::cout << self << " (RSU|onWSM) SEND_CLOUD_EVT " << simTime().dbl() << endl;
        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);

        nextModelID = flexe_msg->getSenderID();
        if (!(std::find(receivedModels.begin(), receivedModels.end(), nextModelID) != receivedModels.end())) {
            receivedModels.push_back(nextModelID);
        }

        // MODEL
        for(int i=0; i < flexe_msg->getTensorsSizeArraySize(); i++){
            tensorSizeVector.push_back(flexe_msg->getTensorsSize(i));
        }
        for(int i=0; i < flexe_msg->getTensorsArraySize(); i++){
            tensorsBytes.push_back(flexe_msg->getTensors(i));
        }

        modelStr = std::string(begin(tensorsBytes), end(tensorsBytes));
        modelsMap.insert(std::make_pair(flexe_msg->getSenderID(), std::make_tuple(modelStr, tensorSizeVector)));
        // MODEL

        // STORE_MODEL
        for(it = modelsMap.begin(); it != modelsMap.end(); ++it){
            std::cout << self << " (RSU|onWSM) STORE_MODEL: " << it->first << endl;
            ModelRequest requestModel;
            requestModel.set_idvehicle(it->first+1);
            requestModel.set_number_of_vehicles(modelsMap.size());
            requestModel.set_num_examples(flexe_msg->getNum_examples());

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
            client->store_model(requestModel);
        }
        // STORE_MODEL

        // CLEAR MEMORY
        std::vector<char>().swap(tensorsBytes);
        tensorsBytes.clear();
        tensorsBytes.shrink_to_fit();

        std::vector<int>().swap(tensorSizeVector);
        tensorSizeVector.clear();
        tensorSizeVector.shrink_to_fit();

        modelStr.clear();
        modelsMap.clear();
        // CLEAR MEMORY

        std::cout << self << " (RSU|onWSM) SEND_CLOUD_EVT " << flexe_msg->getSenderID() << " " << simTime().dbl() << "\n" << endl;
        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl() + roundDeadlineTime, sendCloudEvt);
        }
        break;
    }
    default:{
        std::cout << self << " (RSU|onWSM) The message type was not detected. " << wsm->getKind() << endl;
        break;
    }
    }
}

void SemiSyncRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void SemiSyncRSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) SEND_CLOUD_EVT " << simTime().dbl() << endl;
        vehicleIDs = "";
        std::cout << "Number of Vehicles: " << receivedModels.size() << endl;


        for(kt  = receivedModels.begin(); kt != receivedModels.end(); kt++){
            std::cout << "Select Model from Vehicle-ID: " << *kt << endl;
            vehicleIDs = vehicleIDs + ";" + std::to_string(*kt+1);
            emit(selectIDMetric, *kt);
        }


        if(receivedModels.size() != 0){
            client->aggregate_sync_fit(vehicleIDs);
            client->aggregate_evaluate(self, trainFlag, modelName, batchSize);
            emit(roundMetric, (double)roundDeadlineTime);
        }

        receivedModels.clear();
        std::cout << endl;
        std::cout << endl;

        scheduleAt(simTime().dbl()+uniform(0.0,0.01), sendModelEvt);
        break;
    }
    case SEND_FED_MODEL_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) SEND_FED_MODEL_EVT" << endl;
        FlexeMessage* flexe_msg = new FlexeMessage();
        DemoBaseApplLayer::populateWSM(flexe_msg);

        flexe_msg->setSenderID(self);
        flexe_msg->setKind(SEND_FED_MODEL_EVT);
        flexe_msg->setModelVersion(-1);

        DemoBaseApplLayer::sendDelayedDown(flexe_msg, uniform(0.0,0.001));
        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl() + roundDeadlineTime, sendCloudEvt);
        }
        break;
    }
    case CLOUD_EVT:{
        break;
    }
    default: {
        std::cout << self << " (RSU|handleSelfMsg) - The message type was not detected. "  << msg->getKind() << endl;
        break;
    }
    }
}
