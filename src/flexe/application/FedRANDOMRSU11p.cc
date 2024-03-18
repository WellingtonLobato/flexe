#include "FedRANDOMRSU11p.h"

using namespace veins;
using namespace flexe;

Define_Module(FedRANDOMRSU11p);

FedRANDOMRSU11p::~FedRANDOMRSU11p() {
    cancelAndDelete(clientSelectionEvt);
    cancelAndDelete(sendGlobalModelEvt);
    cancelAndDelete(aggregateModelsEvt);
    for(std::vector<std::string>::iterator it  = datasetVector.begin(); it != datasetVector.end(); it++){
        dataset = *it;
        model = datasetModelMap[dataset];
        client->end(self, model, dataset, scenario, seed, nonIID, idModel, "");
    }

    delete client;
}


void FedRANDOMRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0){
        self = getParentModule()->getIndex();
        //NED VARs
        modelVector = cStringTokenizer(par("model")).asVector();
        datasetVector = cStringTokenizer(par("dataset")).asVector();
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
        computationRequirementVector = cStringTokenizer(par("computationRequirement")).asVector();
        roundDeadline = par("roundDeadline").doubleValue();
        address = par("address").stringValue();
        K = par("K").doubleValue();
        //NED VARs

        //METRICS
        selectClientsMetric = registerSignal("selectClients");
        receivedClientsMetric = registerSignal("receivedClients");
        connectedClientsMetric = registerSignal("connectedClients");
        roundMetric = registerSignal("round");
        //METRICS
    }else if(stage == 1){
        //First Training
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        counter = 0;
        for(std::vector<std::string>::iterator it  = datasetVector.begin(); it != datasetVector.end(); it++){
            dataset = *it;
            model = modelVector[counter];
            computationRequirement = std::stod(computationRequirementVector[counter]);
            std::cout << self << " - Initialize Model " << model << " Dataset " << dataset << " Computation Requirement: " << computationRequirement << endl;
            datasetComputationRequirementMap.insert(std::pair<std::string, double>(dataset, computationRequirement));
            datasetModelMap.insert(std::pair<std::string, std::string>(dataset, model));
            datasetMetricMap.insert(std::pair<std::string, int>(dataset, counter));
            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->initialize_parameters(self, model, dataset, scenario, seed, nonIID, idModel, trainFlag, epochs, batch, numClients, percentDataset, "NULL", "NULL");

            //Loss Train
            sprintf(signalName, "lossTrain%s", dataset.c_str());
            signal = registerSignal(signalName);
            statisticTemplate = getProperties()->get("statisticTemplate", "lossTrainCount");
            getEnvir()->addResultRecorders(this, signal, signalName,  statisticTemplate);
            lossTrainSignals[counter] = signal;
            emit(lossTrainSignals[counter], lossModel);

            //Loss Evaluate
            sprintf(signalName, "lossEvaluate%s", dataset.c_str());
            signal = registerSignal(signalName);
            statisticTemplate = getProperties()->get("statisticTemplate", "lossEvaluateCount");
            getEnvir()->addResultRecorders(this, signal, signalName,  statisticTemplate);
            lossEvaluateSignals[counter] = signal;
            emit(lossEvaluateSignals[counter], lossModel);

            //Accuracy Train
            sprintf(signalName, "accuracyTrain%s", dataset.c_str());
            signal = registerSignal(signalName);
            statisticTemplate = getProperties()->get("statisticTemplate", "accuracyTrainCount");
            getEnvir()->addResultRecorders(this, signal, signalName,  statisticTemplate);
            accuracyTrainSignals[counter] = signal;
            emit(accuracyTrainSignals[counter], accuracyModel);

            //Accuracy Evaluate
            sprintf(signalName, "accuracyEvaluate%s", dataset.c_str());
            signal = registerSignal(signalName);
            statisticTemplate = getProperties()->get("statisticTemplate", "accuracyEvaluateCount");
            getEnvir()->addResultRecorders(this, signal, signalName,  statisticTemplate);
            accuracyEvaluateSignals[counter] = signal;
            emit(accuracyEvaluateSignals[counter], accuracyModel);
            counter++;
        }
        //First Training
        std::srand(time(nullptr));
        std::random_shuffle(std::begin(datasetVector), std::end(datasetVector));
        dataset = datasetVector[0];
        model = datasetModelMap[dataset];
        computationRequirement = datasetComputationRequirementMap[dataset];
        std::cout << self << " - RANDOM Model: " << model << " dataset: " << dataset << " Computation Requirement: " << computationRequirement << endl;
        scheduleAt(simTime().dbl()+0.1, clientSelectionEvt);
    }
}

void FedRANDOMRSU11p::onBSM(DemoSafetyMessage* bsm){
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

void FedRANDOMRSU11p::onWSM(BaseFrame1609_4* wsm){
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

void FedRANDOMRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void FedRANDOMRSU11p::handleSelfMsg(cMessage* msg){
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

            counter = datasetMetricMap[dataset];

            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->aggregate_sync_fit(self, model, dataset, scenario, seed, nonIID, idModel, numClients, numExamplesModels, clientsModels, strategy, "NULL");
            emit(lossTrainSignals[counter], lossModel);
            emit(accuracyTrainSignals[counter], accuracyModel);

            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->aggregate_evaluate(self, model, dataset, scenario, seed, nonIID, idModel, numClients, numExamplesModels, clientsModels, strategy, "NULL");
            emit(lossEvaluateSignals[counter], lossModel);
            emit(accuracyEvaluateSignals[counter], accuracyModel);
        }
        //CLEAR DATA STRUCTURES
        receivedModelsVector.clear();
        receivedNumberExamplesVector.clear();
        //CLEAR DATA STRUCTURES

        //NEW RANDOM MODEL & DATASET
        std::srand(time(nullptr));
        std::random_shuffle(std::begin(datasetVector), std::end(datasetVector));
        dataset = datasetVector[0];
        model = datasetModelMap[dataset];
        computationRequirement = datasetComputationRequirementMap[dataset];
        std::cout << self << " - RANDOM Model: " << model << " dataset: " << dataset << " Computation Requirement: " << computationRequirement << endl;
        //NEW RANDOM MODEL & DATASET

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
