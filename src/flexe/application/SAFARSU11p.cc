#include "SAFARSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(SAFARSU11p);

SAFARSU11p::~SAFARSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(sendModelEvt);
    client->end("");
    delete client;
}


void SAFARSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0){
        self = getParentModule()->getIndex();
        cloudModuleName = par("cloudModuleName").stringValue();
        dataset = par("dataset").stringValue();
        epochs = par("epochs").intValue();
        batchSize = par("batchSize").intValue();
        modelName = par("modelName").stringValue();
        roundDeadlineTime = par("roundDeadlineTime").doubleValue();
        address = par("address").stringValue();
        C = par("C").doubleValue();

        selectIDMetric = registerSignal("selectID");
        roundMetric = registerSignal("round");
    }else if(stage == 1){
        //First Training
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        client->initialize_parameters(self, dataset, modelName, epochs, batchSize);
        //First Training
    }
}

void SAFARSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        break;
    }
    default:{
        std::cout << "RUS onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void SAFARSU11p::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_CLOUD_EVT:{
        std::cout <<"0 RSU - BEACON(SEND_CLOUD_EVT): " << simTime().dbl() << endl;
        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);

        // CACHE
        nextModelID = flexe_msg->getSenderID();
        cache_safa.Put(nextModelID, "MODEL");
        // CACHE

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
            std::cout <<"0 RSU - STORE_MODEL: " << it->first << endl;
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

        std::cout << flexe_msg->getSenderID() << " RSU - SEND_CLOUD_EVT: " << simTime().dbl() << "\n" << endl;
        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl() + roundDeadlineTime, sendCloudEvt);
        }
        break;
    }
    default:{
        std::cout << "RUS onWSM - The message type was not detected." << wsm->getKind() << endl;
        break;
    }
    }
}

void SAFARSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void SAFARSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        std::cout << "RSU handleSelfMsg SEND_CLOUD_EVT " << simTime().dbl() << endl;
        cacheIDs = "";

        cacheKeys = cache_safa.GetKeys();
        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(cacheKeys), std::end(cacheKeys), rng);
        int n_candidates = int(cacheKeys.size()) * C;

        for(kt  = cacheKeys.begin(); kt != cacheKeys.end(); kt++){
            if(n_candidates > 0){
                std::cout << "Vehicle in CACHE ID: " << *kt << endl;
                cacheIDs = cacheIDs + ";" + std::to_string(*kt+1);
                emit(selectIDMetric, *kt);
                n_candidates--;
            }
        }

        client->aggregate_sync_fit(cacheIDs);
        std::cout << endl;
        std::cout << endl;

        client->aggregate_evaluate(self, dataset, modelName, batchSize);
        scheduleAt(simTime().dbl()+uniform(0.0,0.01), sendModelEvt);
        break;
    }
    case SEND_FED_MODEL_EVT:{
        std::cout << "RSU - SEND_FED_MODEL_EVT" << endl;
        FlexeMessage* flexe_msg = new FlexeMessage();
        DemoBaseApplLayer::populateWSM(flexe_msg);

        flexe_msg->setSenderID(self);
        flexe_msg->setKind(SEND_FED_MODEL_EVT);

        DemoBaseApplLayer::sendDelayedDown(flexe_msg, uniform(0.0,0.001));
        emit(roundMetric, (double)roundDeadlineTime);
        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl() + roundDeadlineTime, sendCloudEvt);
        }
        break;
    }
    case CLOUD_EVT:{
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}
