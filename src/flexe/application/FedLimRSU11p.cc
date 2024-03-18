#include "FedLimRSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(FedLimRSU11p);

FedLimRSU11p::~FedLimRSU11p() {
    cancelAndDelete(clientSelectionEvt);
    cancelAndDelete(sendGlobalModelEvt);
    cancelAndDelete(aggregateModelsEvt);
    client->end(self, model, dataset, scenario, seed, nonIID, idModel, "");
    delete client;
}


void FedLimRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0){
        self = getParentModule()->getIndex();
        //NED VARs
        model = par("model").stringValue();
        dataset = par("dataset").stringValue();
        scenario = par("scenario").stringValue();
        seed = par("seed").intValue();
        nonIID = par("nonIID").boolValue();
        idModel = self;
        trainFlag = par("trainFlag").boolValue();
        epochs = par("epochs").intValue();
        batch = par("batch").intValue();
        numClients = par("numClients").intValue();
        percentDataset = par("percentDataset").doubleValue();
        strategy = par("strategy").stringValue();
        computationRequirement = std::stod(par("computationRequirement").stringValue());
        roundDeadline = par("roundDeadline").doubleValue();
        address = par("address").stringValue();
        K = par("K").doubleValue();
        //NED VARs

        //METRICS
        selectClientsMetric = registerSignal("selectClients");
        receivedClientsMetric = registerSignal("receivedClients");
        connectedClientsMetric = registerSignal("connectedClients");
        roundMetric = registerSignal("round");
        lossTrainMetric = registerSignal("lossTrain");
        accuracyTrainMetric = registerSignal("accuracyTrain");
        lossEvaluateMetric = registerSignal("lossEvaluate");
        accuracyEvaluateMetric = registerSignal("accuracyEvaluate");
        //METRICS
    }else if(stage == 1){
        //First Training
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->initialize_parameters(self, model, dataset, scenario, seed, nonIID, idModel, trainFlag, epochs, batch, numClients, percentDataset, "NULL", "NULL");
        emit(lossTrainMetric, (double)lossModel);
        emit(accuracyTrainMetric, (double)accuracyModel);
        emit(lossEvaluateMetric, (double)lossModel);
        emit(accuracyEvaluateMetric, (double)accuracyModel);
        //First Training

        scheduleAt(simTime().dbl()+0.1, clientSelectionEvt);
    }
}

void FedLimRSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        std::vector<std::string> vectorName = client->splitSTR(bsm->getName(), ';');
        std::cout << self << " (RSU|onBSM) Veh-ID: " << vectorName[1] << " SUMO-ID: " << vectorName[0] << " MSG: " << bsm->getName() << endl;
        std::cout << self << " (RSU|onBSM) Veh-ID: " << vectorName[1] << " Speed: " << bsm->getSpeed() << endl;
        connectedClientsMap.insert(std::make_pair(std::stoi(vectorName[1]), vectorName[0]));
        std::cout << "\n" << endl;
        break;
    }
    default:{
        std::cout << self << " (RSU|onBSM) The message type was not detected. " << bsm->getKind() << endl;
        std::cout << "\n" << endl;
        break;
    }
    }
}

void FedLimRSU11p::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_GLOBAL_MODEL_EVT:{
        break;
    }
    case SEND_LOCAL_MODEL_EVT:{
        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);
        std::cout << self << " (RSU|onWSM) SEND_LOCAL_MODEL_EVT " << simTime().dbl() << " Destination: " << flexe_msg->getIdModel() << endl;
        if(idModel == flexe_msg->getIdModel()){
            localModelID = flexe_msg->getSenderID();
            if(!(std::find(receivedModelsVector.begin(), receivedModelsVector.end(), localModelID) != receivedModelsVector.end())){
                receivedModelsVector.push_back(localModelID);
                receivedNumberExamplesVector.push_back(flexe_msg->getNumExamples());
            }
        }
        break;
    }
    default:{
        std::cout << self << " (RSU|onWSM) The message type was not detected. " << wsm->getKind() << endl;
        break;
    }
    }
}

void FedLimRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void FedLimRSU11p::handleSelfMsg(cMessage* msg){
    switch(msg->getKind()){
    case AGGREGATE_MODELS_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) AGGREGATE_MODELS_EVT " << simTime().dbl() << endl;
        if(receivedModelsVector.size() != 0){
            clientsModels = "";
            numExamplesModels = "";
            for(std::vector<int>::iterator it  = receivedModelsVector.begin(); it != receivedModelsVector.end(); it++){
                clientsModels = clientsModels + ";" + std::to_string(*it);
                emit(receivedClientsMetric, (int)*it);
            }
            for(std::vector<int>::iterator it  = receivedNumberExamplesVector.begin(); it != receivedNumberExamplesVector.end(); it++){
                numExamplesModels = numExamplesModels + ";" + std::to_string(*it);
            }
            emit(roundMetric, (double)roundDeadline);

            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->aggregate_sync_fit(self, model, dataset, scenario, seed, nonIID, idModel, numClients, numExamplesModels, clientsModels, strategy, "NULL");
            emit(lossTrainMetric, (double)lossModel);
            emit(accuracyTrainMetric, (double)accuracyModel);

            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->aggregate_evaluate(self, model, dataset, scenario, seed, nonIID, idModel, numClients, numExamplesModels, clientsModels, strategy, "NULL");
            emit(lossEvaluateMetric, (double)lossModel);
            emit(accuracyEvaluateMetric, (double)accuracyModel);
        }
        //CLEAR DATA STRUCTURES
        receivedModelsVector.clear();
        receivedNumberExamplesVector.clear();
        //CLEAR DATA STRUCTURES
        std::cout << "\n" << endl;
        if(!clientSelectionEvt->isScheduled()){
            scheduleAt(simTime().dbl(), clientSelectionEvt);
        }
        break;
    }
    case CLIENT_SELECTION_EVT:{
        std::cout << self << " (RSU|handleSelfMsg) CLIENT_SELECTION_EVT " << simTime().dbl() << endl;
        selectedClients = "";
        if(!connectedClientsMap.empty()){
            std::cout << self << " (RSU|handleSelfMsg) connectedClientsMap NOT EMPTY! " << endl;
            //CLIENT SELECTION
            emit(connectedClientsMetric, (int)connectedClientsMap.size());
            numSelectedClients = std::ceil(connectedClientsMap.size()*K);

            std::cout << self << " (RSU|handleSelfMsg) N. of Connected Clients: " << connectedClientsMap.size() << endl;
            std::cout << self << " (RSU|handleSelfMsg) N. of Selected Clients: " << numSelectedClients << endl;

            for(std::map<int, std::string>::iterator it = connectedClientsMap.begin(); it != connectedClientsMap.end(); ++it) {
                connectedClientsVector.push_back(it->first);
            }

            std::srand(time(nullptr));
            std::random_shuffle(std::begin(connectedClientsVector), std::end(connectedClientsVector));
            counter = 0;
            for(std::vector<int>::iterator it  = connectedClientsVector.begin(); it != connectedClientsVector.end(); it++){
                if(counter >= numSelectedClients){
                    break;
                }
                std::cout << self << " (RSU|handleSelfMsg) Select Model from Vehicle-ID: " << *it << endl;
                selectedClients = selectedClients + ";" + std::to_string(*it);
                emit(selectClientsMetric, *it);
                counter++;
            }
            //CLIENT SELECTION

            //CLEAR DATA STRUCTURES
            connectedClientsMap.clear();
            connectedClientsVector.clear();
            //CLEAR DATA STRUCTURES

            //SEND GLOBAL MODEL
            FlexeMessage* flexe_msg = new FlexeMessage();
            DemoBaseApplLayer::populateWSM(flexe_msg);
            flexe_msg->setKind(SEND_GLOBAL_MODEL_EVT);
            flexe_msg->setSenderID(self);
            flexe_msg->setModel(model.c_str());
            flexe_msg->setDataset(dataset.c_str());
            flexe_msg->setScenario(scenario.c_str());
            flexe_msg->setSeed(seed);
            flexe_msg->setNonIID(nonIID);
            flexe_msg->setIdModel(idModel);
            flexe_msg->setNumClients(numClients);
            flexe_msg->setPercentDataset(percentDataset);
            flexe_msg->setModelVersion(-1);
            flexe_msg->setClientSelection(selectedClients.c_str());
            flexe_msg->setComputationRequirement(computationRequirement);
            DemoBaseApplLayer::sendDown(flexe_msg);
            //SEND GLOBAL MODEL

            scheduleAt(simTime().dbl() + roundDeadline, aggregateModelsEvt);
        }else{
            if(!clientSelectionEvt->isScheduled()){
                scheduleAt(simTime().dbl() + 0.1, clientSelectionEvt);
            }
        }
        std::cout << "\n" << endl;
        break;
    }
    default: {
        std::cout << self << " (RSU|handleSelfMsg) - The message type was not detected. "  << msg->getKind() << endl;
        break;
    }
    }
}
