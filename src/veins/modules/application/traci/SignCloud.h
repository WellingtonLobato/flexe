#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/ml/mlp.h"

namespace veins {

class CloudDatasetSign {
public:
    cv::Mat image;
    int category;
    std::string nome;

    CloudDatasetSign(cv::Mat& image, std::string name) :image(image) {
        category = stoi(name.substr(0, 3));
        nome = name;
    };
};

class VEINS_API SignCloud : public DemoBaseApplLayer {
public:
    ~SignCloud();


protected:
    //MSG TYPE
    enum selfMessageKinds {
        CLOUD_EVT = 12207,
    };
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onBSM(DemoSafetyMessage* bsm) override;
    void handleSelfMsg(cMessage* msg) override;
    std::vector<CloudDatasetSign> loadSignsFromFolder(std::string folderName);
    cv::Mat getInputDataFromSignsVector(std::vector<CloudDatasetSign> roadSigns);
    cv::Mat getOutputDataFromSignsVector(std::vector<CloudDatasetSign> roadSigns);

    //VAR
    int eventoEscalonado = 0;
    int self;
    //MLP|FL VAR
    int numberLayers;
    int numTrainData;
    int hiddenLayer;
    int selfTrainData;
    int minNumberClients;
    int max_iter;
    double eps;
    //Dataset VAR
    int headerLineCount;
    int responseStartIdx;
    int responseEndIdx;
    double splitRatio;
    bool customRead;
    std::string delimiter;

    std::string rsuModuleName;
    std::string modelFile;
    std::string datasetTrainFile;
    std::string datasetTestFile;
    std::string modelMsgAll;
    std::string weight2StrMLP;

    std::vector<std::string> modelMsgVector;

    std::vector<CloudDatasetSign> roadSigns;
    std::vector<CloudDatasetSign> roadSignsTest;
    std::vector<CloudDatasetSign> roadSignsTotal;
    std::vector<CloudDatasetSign> roadSignsVectorCloud;

    std::map<int, std::tuple<int, std::string>> modelsMap;

    cv::Ptr<cv::ml::TrainData> dataset;
    cv::Ptr<cv::ml::TrainData> datasetTest;
    std::list<cv::Mat> FedAVGWeightList;

    cv::Mat inputData;
    cv::Mat outputData;

    //MSG
    cMessage* data2RSU = new cMessage("NULL");;
};

} // namespace veins
