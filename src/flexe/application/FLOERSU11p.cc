#include "FLOERSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(FLOERSU11p);

FLOERSU11p::~FLOERSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(sendModelEvt);
    client->end("");
    delete client;
}

double FLOERSU11p::meanContactTime(std::map<int, std::tuple<double, double>> contactMap){
    std::cout << simTime().dbl()<< " - FLOERSU11p::meanContactTime" << endl;

    double meanTime = 0.0;
    double c = 0.0;
    for(std::map<int, std::tuple<double, double>>::iterator it = contactMap.begin(); it != contactMap.end(); it++){
        std::cout << "Contact ID: " << it->first << " Contact Time: " << std::get<1>(it->second) << " Last Time: " << std::get<0>(it->second) << endl;
        if(std::get<1>(it->second) > 0){
            meanTime = meanTime + std::get<1>(it->second);
            c = c + 1.0;
        }
    }
    std::cout << "meanTime: " << (meanTime/c) << "\n" << endl;
    if(std::isnan(meanTime/c)){
        return -1;
    }else{
        return (meanTime/c);
    }
}

void FLOERSU11p::initialize(int stage){
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
        std::cout << address << endl;

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

void FLOERSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        sourceID = std::atoi(bsm->getName());

        std::map<int, std::tuple<double, double>>::iterator it = contactMap.find(sourceID);
        if(it != contactMap.end()){
            //In map
            diff = simTime().dbl() - std::get<0>(it->second);
            if(diff <= 1.05){
                std::get<0>(it->second) = simTime().dbl();
                std::get<1>(it->second) = std::get<1>(it->second) + diff;
            }else{
                std::get<0>(it->second) = simTime().dbl();
                std::get<1>(it->second) = 0.0;
            }
        }else{
            //Not in map
            contactMap.insert(std::make_pair(sourceID, std::make_tuple(simTime().dbl(), 0.0)));
        }

        std::list<int> listEraseKeys;
        for(std::map<int, std::tuple<double, double>>::iterator it = contactMap.begin(); it != contactMap.end(); it++){
            diff = simTime().dbl() - std::get<0>(it->second);
            if(diff > 1.05){
                std::get<1>(it->second) = 0.0;
                listEraseKeys.push_back(it->first);
            }
        }

        for(std::list<int>::iterator it = listEraseKeys.begin(); it != listEraseKeys.end(); it++){
            contactMap.erase(*it);
        }
        listEraseKeys.clear();
        break;
    }
    default:{
        std::cout << "RUS onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void FLOERSU11p::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_CLOUD_EVT:{
        std::cout <<"0 RSU - BEACON(SEND_CLOUD_EVT): " << simTime().dbl() << endl;

        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);

        // CACHE
        nextModelID = flexe_msg->getSenderID();
        cache.Put(nextModelID, "MODEL");
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

void FLOERSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void FLOERSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        std::cout << "RSU handleSelfMsg SEND_CLOUD_EVT " << simTime().dbl() << endl;

        std::list<int> listEraseKeys;
        for(std::map<int, std::tuple<double, double>>::iterator it = contactMap.begin(); it != contactMap.end(); it++){
            diff = simTime().dbl() - std::get<0>(it->second);
            if(diff > 1.1){
                std::get<1>(it->second) = 0.0;
                listEraseKeys.push_back(it->first);
            }
        }
        for(std::list<int>::iterator it = listEraseKeys.begin(); it != listEraseKeys.end(); it++){
            contactMap.erase(*it);
        }
        listEraseKeys.clear();

        roundDeadlineTime = meanContactTime(contactMap);
        std::cout << "NEW roundDeadlineTime: " << roundDeadlineTime << endl;

        cacheIDs = "";

        cacheKeys = cache.GetKeys();
        for(kt  = cacheKeys.begin(); kt != cacheKeys.end(); kt++){
            std::cout << "Vehicle in CACHE ID: " << *kt << endl;
            cacheIDs = cacheIDs + ";" + std::to_string(*kt+1);
            emit(selectIDMetric, *kt);
        }

        client->aggregate_sync_fit(cacheIDs);

        std::cout << endl;
        std::cout << endl;

        if(roundDeadlineTime == -1){
            roundDeadlineTime = par("roundDeadlineTime").doubleValue();
            std::cout << "Default RoundDeadlineTime: " << roundDeadlineTime << endl;
        }

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
