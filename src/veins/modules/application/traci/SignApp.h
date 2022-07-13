#pragma once

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/ml/mlp.h"

using namespace omnetpp;

namespace veins {

/**
 *
 * @author Wellington Lobato
 *
 */

class SignDataset {
public:
    cv::Mat image;
    unsigned short int category;

    SignDataset(cv::Mat& image, std::string name) :image(image) {
        category = stoi(name.substr(0, 3));
    };
};

class VEINS_API SignApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~SignApp();

protected:
    //MSG TYPE
    enum selfMessageKinds {
        TRAIN_MODEL_EVT=1,
        SEND_MODEL_EVT,
        SEND_CLOUD_EVT = 10,
        SEND_FED_MODEL_EVT = 17,
    };
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    std::vector<SignDataset> loadSignsFromFolder(std::string folderName);
    cv::Mat getInputDataFromSignsVector(std::vector<SignDataset> roadSigns);
    cv::Mat getOutputDataFromSignsVector(std::vector<SignDataset> roadSigns);

    //VAR
    int self;
    int numberLayers;
    int numTrainData;
    int max_iter;
    int hiddenLayer;
    int numRounds;
    double eps;
    //Dataset VAR
    int headerLineCount;
    int responseStartIdx;
    int responseEndIdx;
    double splitRatio;
    double splitClientRatio;
    bool nonIID;
    bool customRead;
    std::string delimiter;
    double trainInterval;

    std::string modelFile;
    std::string cloudModelFile;
    std::string datasetTrainFile;
    std::string datasetTestFile;

    std::vector<SignDataset> roadSigns;
    std::vector<SignDataset> roadSignsVector;
    std::vector<SignDataset> roadSignsTest;
    std::vector<SignDataset> roadSignsTotal;

    cv::Mat inputTrainingData;
    cv::Mat outputTrainingData;
    cv::Mat inputTestData;
    cv::Mat outputTestData;
    cv::Mat inputData;
    cv::Mat outputData;

    static cv::Ptr<cv::ml::TrainData> dataset;
    static cv::Ptr<cv::ml::TrainData> datasetTest;
    static int binderCounter;

    std::list<cv::Mat> weightListMLP;
    std::list<std::string> weight2StrListMLP;
    std::list<cv::Mat> str2WeightListMLP;
    std::string weight2StrMLP;

    //MSG
    cMessage* trainModelEvt;
    cMessage* testCloudModelEvt;
    cMessage* sendModelEvt;
};

} // namespace veins
