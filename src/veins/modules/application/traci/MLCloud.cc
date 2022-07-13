#include "veins/modules/application/traci/MLCloud.h"

using namespace veins;

Define_Module(veins::MLCloud);

MLCloud::~MLCloud() {
    cancelAndDelete(data2RSU);
}

void MLCloud::onWSA(DemoServiceAdvertisment* wsa){
}

void MLCloud::onWSM(BaseFrame1609_4* frame){
}


void MLCloud::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    default:{
        break;
    }
    }
}

void MLCloud::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case CLOUD_EVT:{
        if(eventoEscalonado == 0){
            rsuModuleName = par("rsuModuleName").stringValue();
            minNumberClients = par("minNumberClients").intValue();
            modelFile = par("modelFile").stringValue();
            modelFile = modelFile + "FedAVG.txt";
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

            if(responseStartIdx == -1 || responseEndIdx == -1){
                std::cout << "DATASET INFO: NOT DEFINED! responseEndIdx or responseEndIdx equals -1" << endl;
                break;
            }

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

            eventoEscalonado++;
        }
        MLP modelMLP(dataset->getNVars(), dataset->getResponses().cols, numberLayers, hiddenLayer, max_iter, eps);

        modelMsgAll = msg->getName();
        modelMsgVector = modelMLP.split(modelMsgAll, "$");
        weight2StrMLP = modelMsgVector[0];
        numTrainData = atoi(modelMsgVector[1].c_str());
        selfTrainData = atoi(modelMsgVector[2].c_str());

        modelsMap.insert(std::make_pair(selfTrainData, std::make_tuple(numTrainData, weight2StrMLP)));

        if(modelsMap.size() >= minNumberClients){
            FedAVGWeightList = modelMLP.fedAVG(modelsMap, modelFile);
            modelMLP.testMLP(datasetTest, modelFile, modelFile, splitRatio);
        }
        weight2StrMLP = modelMLP.convertWeightsToStr(FedAVGWeightList);

        //SEND WEIGTHS TO RSU
        if(rsuModuleName.compare("NO_NAME") != 0){
            data2RSU->setName(weight2StrMLP.c_str());
            data2RSU->setKind(CLOUD_EVT);
            DemoBaseApplLayer::sendCloud(data2RSU->dup(), rsuModuleName);
        }else{
            std::cout << "RSU name not defined." << endl;
        }
        //SEND WEIGTHS TO RSU

        break;
    }
    default: {
        break;
    }
    }
}

