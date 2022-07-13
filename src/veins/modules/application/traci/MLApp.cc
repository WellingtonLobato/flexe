#include "veins/modules/application/traci/MLApp.h"

using namespace veins;

Define_Module(veins::MLApp);

void MLApp::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        self = getParentModule()->getIndex();
        numRounds = par("numRounds").intValue();
        modelFile = par("modelFile").stringValue();
        datasetTrainFile = par("datasetTrainFile").stringValue();
        datasetTestFile = par("datasetTestFile").stringValue();
        customRead = par("customRead").boolValue();
        delimiter = par("delimiter").stringValue();
        numberLayers = par("numberLayers").intValue();
        hiddenLayer = par("hiddenLayer").intValue();
        max_iter = par("max_iter").intValue();
        eps = par("eps").doubleValue();

        splitRatio = par("splitRatio").doubleValue();
        headerLineCount = par("headerLineCount").intValue();
        responseStartIdx = par("responseStartIdx").intValue();
        responseEndIdx = par("responseEndIdx").intValue();

        trainModelEvt = new cMessage("Send Model to Cloud Layer Event", TRAIN_MODEL_EVT);
        sendModelEvt = new cMessage("Send Model to Cloud Layer Event", SEND_MODEL_EVT);

        //READ DATASET
        if(!delimiter.compare(",") && customRead == false){
            dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx);
            datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx);
        }else if(customRead == false){
            dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
            datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
        }else{
            //CUSTOM DATASET READ
        }
        //READ DATASET
    }
    else if (stage == 1) {
        scheduleAt(simTime().dbl(), trainModelEvt);
    }
}

MLApp::~MLApp() {
    cancelAndDelete(sendModelEvt);
    cancelAndDelete(trainModelEvt);
}

void MLApp::finish(){
    DemoBaseApplLayer::finish();
}

void MLApp::onBSM(DemoSafetyMessage* bsm){
}

void MLApp::onWSM(BaseFrame1609_4* wsm){
}

void MLApp::onWSA(DemoServiceAdvertisment* wsa){
}

void MLApp::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case TRAIN_MODEL_EVT:{
        if(responseStartIdx == -1 || responseEndIdx == -1){
            std::cout << "DATASET INFO: NOT DEFINED! responseEndIdx or responseEndIdx equals -1" << endl;
            break;
        }

        MLP modelMLP(dataset->getNVars(), dataset->getResponses().cols, numberLayers, hiddenLayer, max_iter, eps);
        std::tie(numTrainData, weight2StrMLP) = modelMLP.trainMLP(dataset, modelFile+std::to_string(self)+".txt", splitRatio);
        modelMLP.testMLP(datasetTest, modelFile+std::to_string(self)+".txt", modelFile+std::to_string(self)+".txt", splitRatio);

        scheduleAt(simTime().dbl(), sendModelEvt);
        break;
    }
    case SEND_MODEL_EVT:{
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        DemoBaseApplLayer::populateWSM(bsm);
        bsm->setModelData(weight2StrMLP.c_str());
        bsm->setKind(BEACON_MSG_EVT);
        bsm->setNumTrainData(numTrainData);
        bsm->setSourceAddress(self);
        DemoBaseApplLayer::sendDelayedDown(bsm, uniform(0.0,1));
        if(numRounds != 0){
            scheduleAt(simTime().dbl() + beaconInterval, trainModelEvt);
            //std::cout << self << " - ROUND " << numRounds << endl;
            numRounds--;
        }
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

void MLApp::handlePositionUpdate(cObject* obj){
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
