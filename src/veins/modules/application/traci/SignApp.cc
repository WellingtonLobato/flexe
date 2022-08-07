#include "veins/modules/application/traci/SignApp.h"

using namespace veins;

Define_Module(veins::SignApp);

int SignApp::binderCounter = 0;
cv::Ptr<cv::ml::TrainData> SignApp::dataset = cv::Ptr<cv::ml::TrainData>();
cv::Ptr<cv::ml::TrainData> SignApp::datasetTest = cv::Ptr<cv::ml::TrainData>();

std::vector<SignDataset> SignApp::loadSignsFromFolder(std::string folderName){
    MLP dummyMLP(0, 0, 0, 0, 0, 0);
    for (std::string fileName : dummyMLP.getFilesNamesInFolder(folderName, ".png")) {
        cv::Mat image;
        dummyMLP.loadImage(folderName + fileName, image);
        roadSignsVector.emplace_back(image, fileName.substr(0, (fileName.length() - 4)));
    }
    return roadSignsVector;
}

cv::Mat SignApp::getInputDataFromSignsVector(vector<SignDataset> roadSigns){
    cv::Mat roadSignsImageData;
    for (SignDataset sign : roadSigns) {
        cv::Mat signImageDataInOneRow = sign.image.reshape(0, 1);
        roadSignsImageData.push_back(signImageDataInOneRow);
    }
    return roadSignsImageData;
}

cv::Mat SignApp::getOutputDataFromSignsVector(std::vector<SignDataset> roadSigns){
    int maxCategory = -1;
    for (SignDataset sign : roadSigns) {
        if(sign.category > maxCategory){
            maxCategory = sign.category;
        }
    }
    int signsCount = (int) roadSigns.size();
    int signsVectorSize = signsCount + 1;

    cv::Mat roadSignsData(0, signsVectorSize, CV_32FC1);

    for (SignDataset sign : roadSigns) {
        std::vector<float> outputTraningVector(maxCategory+1);
        fill(outputTraningVector.begin(), outputTraningVector.end(), 0.0);
        outputTraningVector[sign.category] = 1.0;

        cv::Mat tempMatrix(outputTraningVector, false);
        roadSignsData.push_back(tempMatrix.reshape(0, 1));
    }

    return roadSignsData;
}

void SignApp::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        self = getParentModule()->getIndex();
        numRounds = par("numRounds").intValue();
        modelFile = par("modelFile").stringValue();
        cloudModelFile = modelFile + "FedAVG.txt";
        datasetTrainFile = par("datasetTrainFile").stringValue();
        datasetTestFile = par("datasetTestFile").stringValue();
        customRead = par("customRead").boolValue();
        nonIID = par("nonIID").boolValue();
        delimiter = par("delimiter").stringValue();
        numberLayers = par("numberLayers").intValue();
        hiddenLayer = par("hiddenLayer").intValue();
        max_iter = par("max_iter").intValue();
        eps = par("eps").doubleValue();
        trainInterval = par("trainInterval").doubleValue();

        splitRatio = par("splitRatio").doubleValue();
        splitClientRatio = par("splitClientRatio").doubleValue();
        headerLineCount = par("headerLineCount").intValue();
        responseStartIdx = par("responseStartIdx").intValue();
        responseEndIdx = par("responseEndIdx").intValue();

        trainModelEvt = new cMessage("Send Model to Cloud Layer Event", TRAIN_MODEL_EVT);
        sendModelEvt = new cMessage("Send Model to Cloud Layer Event", SEND_MODEL_EVT);
        testCloudModelEvt = new cMessage("Test Federated Model from Cloud Layer Event", SEND_FED_MODEL_EVT);
        if(binderCounter == 0){
            //READ DATASET
            std::cout << self << " - TRAIN_MODEL_EVT: READ DATASET -" << endl;
            if(!delimiter.compare(",") && customRead == false){
                dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx);
                datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx);
            }else if(customRead == false){
                dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
                datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
            }else{
                //CUSTOM DATASET READ
                MLP dummyMLP(0, 0, 0, 0, 0, 0);
                //TRAIN DATASET
                std::vector<std::string> fileNameSplit = dummyMLP.split(datasetTrainFile, "/");

                fileNameSplit[fileNameSplit.size()-1] = "";
                std::string imageFolder = "/";
                for(std::vector<std::string>::iterator it = fileNameSplit.begin(); it != fileNameSplit.end(); ++it){
                    imageFolder = imageFolder + *it + "/";
                }
                imageFolder = imageFolder.substr(0, imageFolder.size()-1);
                roadSigns = loadSignsFromFolder(imageFolder);
                std::vector<SignDataset>().swap(roadSignsVector);
                //TRAIN DATASET

                //TEST DATASET
                std::vector<std::string> fileNameSplitTest = dummyMLP.split(datasetTestFile, "/");

                fileNameSplitTest[fileNameSplitTest.size()-1] = "";
                std::string imageFolderTest = "/";
                for(std::vector<std::string>::iterator it = fileNameSplitTest.begin(); it != fileNameSplitTest.end(); ++it){
                    imageFolderTest = imageFolderTest + *it + "/";
                }
                imageFolderTest = imageFolderTest.substr(0, imageFolderTest.size()-1);
                roadSignsTest = loadSignsFromFolder(imageFolderTest);
                std::vector<SignDataset>().swap(roadSignsVector);
                //TEST DATASET

                roadSignsTotal.reserve(roadSigns.size()+roadSignsTest.size());
                roadSignsTotal.insert(roadSignsTotal.end(), roadSigns.begin(), roadSigns.end());
                roadSignsTotal.insert(roadSignsTotal.end(), roadSignsTest.begin(), roadSignsTest.end());

                inputData = getInputDataFromSignsVector(roadSignsTotal);
                outputData = getOutputDataFromSignsVector(roadSignsTotal);

                cv::Ptr<cv::ml::TrainData> trainDataset = cv::ml::TrainData::create(
                        inputData,
                        cv::ml::SampleTypes::ROW_SAMPLE,
                        outputData);
                dataset = trainDataset;
                //CUSTOM DATASET READ
            }
            //READ DATASET
        }
        ++binderCounter;

    }
    else if (stage == 1) {
        scheduleAt(simTime().dbl()+1.0+uniform(0.0,0.1), trainModelEvt);
        scheduleAt(simTime().dbl()+0.1, sendBeaconEvt);
    }
}

SignApp::~SignApp() {
    cancelAndDelete(sendModelEvt);
    cancelAndDelete(trainModelEvt);
    cancelAndDelete(testCloudModelEvt);
}

void SignApp::finish(){
    DemoBaseApplLayer::finish();
}

void SignApp::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_FED_MODEL_EVT:{
        if(!testCloudModelEvt->isScheduled()){
            scheduleAt(simTime().dbl()+1.0, testCloudModelEvt);
        }
        break;
    }
    default:{
        //std::cout << "CAV onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void SignApp::onWSM(BaseFrame1609_4* wsm){
}

void SignApp::onWSA(DemoServiceAdvertisment* wsa){
}

void SignApp::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_BEACON_EVT:{
        std::cout << self << " - SEND_BEACON_EVT: " << beaconInterval << endl;
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        populateWSM(bsm);
        sendDown(bsm);
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
    }
    case TRAIN_MODEL_EVT:{
        std::cout << self << " - TRAIN_MODEL_EVT: " << simTime().dbl()<< endl;
        MLP modelMLP(SignApp::dataset->getSamples().cols, SignApp::dataset->getResponses().cols, numberLayers, hiddenLayer, max_iter, eps);
        std::tie(numTrainData, weight2StrMLP) = modelMLP.trainMLP(SignApp::dataset, modelFile+std::to_string(self)+".txt", splitRatio);
        modelMLP.testMLP(SignApp::dataset, modelFile+std::to_string(self)+".txt", modelFile+std::to_string(self)+".txt", splitRatio);

        /*
        //RELEASE MEMORY
        cv::Ptr<cv::ml::TrainData>().swap(dataset);
        cv::Ptr<cv::ml::TrainData>().swap(datasetTest);

        inputData.release();
        outputData.release();

        std::vector<SignDataset>().swap(roadSignsTotal);
        std::vector<SignDataset>().swap(roadSignsTest);
        std::vector<SignDataset>().swap(roadSigns);
        //RELEASE MEMORY
         */

        scheduleAt(simTime().dbl(), sendModelEvt);
        break;
    }
    case SEND_MODEL_EVT:{
        std::cout << self << " - SignApp::handleSelfMsg::SEND_MODEL_EVT - ROUND: " << numRounds << endl;
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        DemoBaseApplLayer::populateWSM(bsm);
        bsm->setModelData(weight2StrMLP.c_str());
        bsm->setKind(SEND_CLOUD_EVT);
        bsm->setNumTrainData(numTrainData);
        bsm->setSourceAddress(self);
        DemoBaseApplLayer::sendDelayedDown(bsm, uniform(0.0,0.1));
        if(numRounds > 0){
            scheduleAt(simTime().dbl() + trainInterval, trainModelEvt);
            numRounds--;
        }
        break;
    }
    case SEND_FED_MODEL_EVT:{
        MLP modelMLP(SignApp::dataset->getSamples().cols, SignApp::dataset->getResponses().cols, numberLayers, hiddenLayer, max_iter, eps);
        modelMLP.testMLP(SignApp::dataset, cloudModelFile, modelFile+std::to_string(self)+"_Fed.txt", splitRatio);
        break;
    }
    default: {
        std::cout << self << " handleSelfMsg - The message type was not detected. " << msg->getKind() << endl;
        break;
    }
    }
}

void SignApp::handlePositionUpdate(cObject* obj){
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
