#include "veins/modules/application/traci/SignCloud.h"

using namespace veins;

Define_Module(veins::SignCloud);

std::vector<CloudDatasetSign> SignCloud::loadSignsFromFolder(std::string folderName){
    MLP insideMLP(0, 0, 0, 0, 0, 0);
    for (std::string fileName : insideMLP.getFilesNamesInFolder(folderName, ".png")) {
        cv::Mat image;
        insideMLP.loadImage(folderName + fileName, image);
        roadSignsVectorCloud.emplace_back(image, fileName.substr(0, (fileName.length() - 4)));
    }
    //std::vector<CloudDatasetSign>().swap(roadSignsVectorCloud);
    return roadSignsVectorCloud;
}

cv::Mat SignCloud::getInputDataFromSignsVector(vector<CloudDatasetSign> roadSigns){
    cv::Mat roadSignsImageData;
    for (CloudDatasetSign sign : roadSigns) {
        cv::Mat signImageDataInOneRow = sign.image.reshape(0, 1);
        roadSignsImageData.push_back(signImageDataInOneRow);
    }
    return roadSignsImageData;
}

cv::Mat SignCloud::getOutputDataFromSignsVector(std::vector<CloudDatasetSign> roadSigns){
    int maxCategory = -1;
    for (CloudDatasetSign sign : roadSigns) {
        if(sign.category > maxCategory){
            maxCategory = sign.category;
        }
    }
    int signsCount = (int) roadSigns.size();
    int signsVectorSize = signsCount + 1;

    cv::Mat roadSignsData(0, signsVectorSize, CV_32FC1);

    for (CloudDatasetSign sign : roadSigns) {
        std::vector<float> outputTraningVector(maxCategory+1);
        fill(outputTraningVector.begin(), outputTraningVector.end(), 0.0);
        outputTraningVector[sign.category] = 1.0;

        cv::Mat tempMatrix(outputTraningVector, false);
        roadSignsData.push_back(tempMatrix.reshape(0, 1));
    }

    return roadSignsData;
}

SignCloud::~SignCloud() {
    cancelAndDelete(data2RSU);
}

void SignCloud::onWSA(DemoServiceAdvertisment* wsa){
}

void SignCloud::onWSM(BaseFrame1609_4* frame){
}


void SignCloud::onBSM(DemoSafetyMessage* bsm){
}

void SignCloud::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case CLOUD_EVT:{
        //eventoEscalonado++; //Tirar (Colocar apenas para deixar mais rápido)
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

            //READ DATASET
            if(!delimiter.compare(",") && customRead == false){
                dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx);
                datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx);
            }else if(customRead == false){
                dataset = cv::ml::TrainData::loadFromCSV(datasetTrainFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
                datasetTest = cv::ml::TrainData::loadFromCSV(datasetTestFile, headerLineCount, responseStartIdx, responseEndIdx, delimiter.c_str());
            }else{
                std::cout << "Cloud - SignCloud::handleSelfMsg::CUSTOM DATASET READ: " << endl;
                //CUSTOM DATASET READ
                MLP dummyMLP(0, 0, 0, 0, 0, 0);

                //TRAIN DATASET
                std::vector<std::string> fileNameSplit = dummyMLP.split(datasetTrainFile, "/");
                std::cout << "1 " << datasetTrainFile <<  endl;
                fileNameSplit[fileNameSplit.size()-1] = "";
                std::string imageFolder = "/";
                for(std::vector<std::string>::iterator it = fileNameSplit.begin(); it != fileNameSplit.end(); ++it){
                    imageFolder = imageFolder + *it + "/";
                }
                std::cout << "? " <<  endl;
                imageFolder = imageFolder.substr(0, imageFolder.size()-1);
                std::cout << "??" <<  endl;
                roadSigns = loadSignsFromFolder(imageFolder);
                std::cout << "???" <<  endl;
                std::vector<CloudDatasetSign>().swap(roadSignsVectorCloud);
                std::cout << "1" << endl;
                //TRAIN DATASET

                //TEST DATASET
                std::cout << "2" << endl;
                std::vector<std::string> fileNameSplitTest = dummyMLP.split(datasetTestFile, "/");

                fileNameSplitTest[fileNameSplitTest.size()-1] = "";
                std::string imageFolderTest = "/";
                for(std::vector<std::string>::iterator it = fileNameSplitTest.begin(); it != fileNameSplitTest.end(); ++it){
                    imageFolderTest = imageFolderTest + *it + "/";
                }
                imageFolderTest = imageFolderTest.substr(0, imageFolderTest.size()-1);
                roadSignsTest = loadSignsFromFolder(imageFolderTest);
                std::vector<CloudDatasetSign>().swap(roadSignsVectorCloud);
                std::cout << "2" << endl;
                //TEST DATASET

                std::cout << "3" << endl;
                roadSignsTotal.reserve(roadSigns.size()+roadSignsTest.size());
                roadSignsTotal.insert(roadSignsTotal.end(), roadSigns.begin(), roadSigns.end());
                roadSignsTotal.insert(roadSignsTotal.end(), roadSignsTest.begin(), roadSignsTest.end());
                std::cout << "33" << endl;
                std::vector<CloudDatasetSign>().swap(roadSigns);
                std::vector<CloudDatasetSign>().swap(roadSignsTest);

                inputData = getInputDataFromSignsVector(roadSignsTotal);
                outputData = getOutputDataFromSignsVector(roadSignsTotal);


                cv::Ptr<cv::ml::TrainData> trainDataset = cv::ml::TrainData::create(
                        inputData,
                        cv::ml::SampleTypes::ROW_SAMPLE,
                        outputData);
                dataset = trainDataset;

                inputData.release();
                outputData.release();

                std::vector<CloudDatasetSign>().swap(roadSignsTotal);
                std::cout << "3!" << endl;
                /**/
                //CUSTOM DATASET READ
            }
            //READ DATASET
            eventoEscalonado++;
        }

        MLP modelMLP(dataset->getSamples().cols, dataset->getResponses().cols, numberLayers, hiddenLayer, max_iter, eps);
        modelMsgAll = msg->getName();
        modelMsgVector = modelMLP.split(modelMsgAll, "$");
        weight2StrMLP = modelMsgVector[0];
        numTrainData = atoi(modelMsgVector[1].c_str());
        selfTrainData = atoi(modelMsgVector[2].c_str());

        modelsMap.insert(std::make_pair(selfTrainData, std::make_tuple(numTrainData, weight2StrMLP)));
        std::cout << "Cloud - SignCloud::handleSelfMsg:: - modelsMap.size(): " << modelsMap.size() << " minNumberClients: " << minNumberClients << endl;
        if(modelsMap.size() >= minNumberClients){
            std::cout << "Cloud - SignCloud::handleSelfMsg::modelsMap.size() >= minNumberClients -" << endl;
            FedAVGWeightList = modelMLP.fedAVG(modelsMap, modelFile);
            modelMLP.testMLP(dataset, modelFile, modelFile, splitRatio);
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
        }
        /**/
        break;
    }
    default: {
        break;
    }
    }
}

