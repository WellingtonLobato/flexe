#include "MLApp.h"

using namespace veins;
using namespace flexe;

Define_Module(MLApp);

void MLApp::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        self = getParentModule()->getIndex();
        
        epochs = par("epochs").intValue();
        batch = par("batch").intValue();
        speedZero = par("speedZero").boolValue();
        computationCapability = par("computationCapability").doubleValue();
        computationCapability = computationCapability*(pow(10, 9));

        address = par("address").stringValue();

        trainLatencyMetric = registerSignal("trainLatency");
        compCapabilityMetric = registerSignal("compCapability");
        lossTrainMetric = registerSignal("lossTrain");
        accuracyTrainMetric = registerSignal("accuracyTrain");
        lossEvaluateMetric = registerSignal("lossEvaluate");
        accuracyEvaluateMetric = registerSignal("accuracyEvaluate");
        lossUpdateMetric = registerSignal("lossUpdate");
        accuracyUpdateMetric = registerSignal("accuracyUpdate");

        emit(compCapabilityMetric, (double)computationCapability);
    }
    else if (stage == 1) {
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        computationRequirement = -1;
        modelVersion = -1;
        accuracyModel = -1;
        lossModel = -1;
        numExamples = -1;
        entropy = -1;
    }
}

MLApp::~MLApp(){
    cancelAndDelete(trainLocalModelEvt);
    cancelAndDelete(sendResourceRequestEvt);
    delete client;
}

void MLApp::finish(){
    DemoBaseApplLayer::finish();
}

void MLApp::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        break;
    }
    default:{
        std::cout << self << " - (onBSM) The message type was not detected. " << bsm->getKind() << endl;
        std::cout << "\n" << endl;
        break;
    }
    }
}

void MLApp::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
    case SEND_GLOBAL_MODEL_EVT:{
        std::cout << self << " - (VEH|onWSM) SEND_GLOBAL_MODEL_EVT " << simTime().dbl() << endl;
        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);
        std::cout << self << " - (VEH|onWSM) Selected Clients: " << flexe_msg->getClientSelection() << endl;
        std::vector<std::string> selectedClientsVector = client->splitSTR(flexe_msg->getClientSelection(), ';');
        trainFlag = false;
        
        model = flexe_msg->getModel();
        dataset = flexe_msg->getDataset();
        scenario = flexe_msg->getScenario();
        seed = flexe_msg->getSeed();
        nonIID = flexe_msg->getNonIID();
        idModel = flexe_msg->getIdModel();
        numClients = flexe_msg->getNumClients();
        percentDataset = flexe_msg->getPercentDataset();
        modelVersion = flexe_msg->getModelVersion();
        computationRequirement = flexe_msg->getComputationRequirement();
        
        for(std::vector<std::string>::iterator it  = selectedClientsVector.begin(); it != selectedClientsVector.end(); it++){
            if(std::to_string(self) == *it){
                std::cout << self << " - (VEH|onWSM) " << self << " == "<< *it << endl;
                trainFlag = true;
                break;
            }
        }
        if(trainFlag == true){
            scheduleAt(simTime().dbl(), trainLocalModelEvt);
        }else{ 
            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->update_model(self, model, dataset, scenario, seed, nonIID, idModel, trainFlag, epochs, batch, numClients, percentDataset, "", "");
            emit(lossUpdateMetric, (double)lossModel);
            emit(accuracyUpdateMetric, (double)accuracyModel);           
            std::cout << self << " - (VEH|handleSelfMsg) Update Accuracy: " << accuracyModel << " Loss: " << lossModel << " N. Examples: " << numExamples << endl;
        }
        std::cout << "\n" << endl;

        //CLEAR DATA STRUCTURES
        selectedClientsVector.clear();
        //CLEAR DATA STRUCTURES
        break;
    }
    case SEND_LOCAL_MODEL_EVT:{
        break;
    }
    case RESOURCE_REQUEST_EVT:{
        std::cout << self << " - (VEH|onWSM) RESOURCE_REQUEST_EVT " << simTime().dbl() << endl;
        FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);
        std::vector<std::string> clientResourceVector = client->splitSTR(flexe_msg->getClientSelection(), ';');
        resourceFlag = false;
        for(std::vector<std::string>::iterator it  = clientResourceVector.begin(); it != clientResourceVector.end(); it++){
            if(std::to_string(self) == *it){
                std::cout << self << " - (VEH|onWSM) " << self << " == "<< *it << endl;
                resourceFlag = true;
                break;
            }
        }
        if(resourceFlag == true){
            scheduleAt(simTime().dbl(), sendResourceRequestEvt);
        }
        //CLEAR DATA STRUCTURES
        clientResourceVector.clear();
        //CLEAR DATA STRUCTURES
        break;
    }
    default:{
        std::cout << self << " - (onWSM) The message type was not detected. " << wsm->getKind() << endl;
        std::cout << "\n" << endl;
        break;
    }
    }
}

void MLApp::onWSA(DemoServiceAdvertisment* wsa){
}

void MLApp::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
        case SEND_BEACON_EVT:{
            std::cout << self << " - (VEH|handleSelfMsg) SEND_BEACON_EVT " << simTime().dbl() << endl;
            DemoSafetyMessage* bsm = new DemoSafetyMessage();
            populateWSM(bsm);
            std::string sendID = traciVehicle->getVehicleId() + ";" + std::to_string(self);
            bsm->setName(sendID.c_str());
            bsm->setSpeed(traciVehicle->getSpeed());
            bsm->setModelVersion(modelVersion);
            bsm->setComputationCapability(computationCapability);
            sendDown(bsm);
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            std::cout << "\n" << endl;
            break;
        }

        case TRAIN_LOCAL_MODEL_EVT:{
            std::cout << self << " - (VEH|handleSelfMsg) TRAIN_LOCAL_MODEL_EVT " << simTime().dbl() << endl;
            
            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->fit(self, model, dataset, scenario, seed, nonIID, idModel, trainFlag, epochs, batch, numClients, percentDataset, "", "");
            emit(lossTrainMetric, (double)lossModel);
            emit(accuracyTrainMetric, (double)accuracyModel);
            std::cout << self << " - (VEH|handleSelfMsg) Training Accuracy: " << accuracyModel << " Loss: " << lossModel << " N. Examples: " << numExamples << endl;

            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->evaluate(self, model, dataset, scenario, seed, nonIID, idModel, trainFlag, epochs, batch, numClients, percentDataset, "", "");
            emit(lossEvaluateMetric, (double)lossModel);
            emit(accuracyEvaluateMetric, (double)accuracyModel);           
            std::cout << self << " - (VEH|handleSelfMsg) Evaluation Accuracy: " << accuracyModel << " Loss: " << lossModel << " N. Examples: " << numExamples << endl;
            
            trainingLatency = ((double(numExamples)* computationRequirement)/computationCapability) * epochs;
            emit(trainLatencyMetric, (double)trainingLatency);

            //SEND_LOCAL_MODEL
            FlexeMessage* flexe_msg = new FlexeMessage();
            DemoBaseApplLayer::populateWSM(flexe_msg);
            flexe_msg->setKind(SEND_LOCAL_MODEL_EVT);
            flexe_msg->setSenderID(self);
            flexe_msg->setNumExamples(numExamples);
            flexe_msg->setIdModel(idModel);
            flexe_msg->setModelVersion(modelVersion);
            flexe_msg->setLoss(lossModel);
            flexe_msg->setAccuracy(accuracyModel);
            flexe_msg->setComputationCapability(computationCapability);
            flexe_msg->setEntropy(entropy);
            DemoBaseApplLayer::sendDelayedDown(flexe_msg, trainingLatency);
            //SEND_LOCAL_MODEL

            trainFlag = false;
            std::cout << "\n" << endl;
            break;
        }
        case RESOURCE_REQUEST_EVT:{
            std::cout << self << " - (VEH|handleSelfMsg) RESOURCE_REQUEST_EVT " << simTime().dbl() << endl;
            std::tie(numExamples, entropy, lossModel, accuracyModel, outputMSG) = client->get_information(self, model, dataset, scenario, seed, nonIID, idModel, "");
            
            FlexeMessage* flexe_msg = new FlexeMessage();
            DemoBaseApplLayer::populateWSM(flexe_msg);
            flexe_msg->setKind(RESOURCE_REQUEST_EVT);
            flexe_msg->setSenderID(self);
            flexe_msg->setNumExamples(numExamples);
            flexe_msg->setComputationCapability(computationCapability);
            flexe_msg->setEntropy(entropy);
            DemoBaseApplLayer::sendDelayedDown(flexe_msg, uniform(0, 0.01));

            resourceFlag = false;
            std::cout << "\n" << endl;
            break;
        }
        default: {
            std::cout << self << " handleSelfMsg - The message type was not detected. " << msg->getKind() << endl;
            break;
        }
    }
}

void MLApp::handlePositionUpdate(cObject* obj){
    if(speedZero){
        traciVehicle->setSpeed(0.0);
    }
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
