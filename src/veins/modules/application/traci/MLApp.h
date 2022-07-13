#pragma once

#include <iomanip>      // std::setprecision
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

class VEINS_API MLApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~MLApp();

protected:
    //MSG TYPE
    enum selfMessageKinds {
        BEACON_MSG_EVT,
        TRAIN_MODEL_EVT,
        SEND_MODEL_EVT,
    };
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

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
    bool customRead;
    std::string delimiter;

    std::string modelFile;
    std::string datasetTrainFile;
    std::string datasetTestFile;

    cv::Ptr<cv::ml::TrainData> dataset;
    cv::Ptr<cv::ml::TrainData> datasetTest;
    std::list<cv::Mat> weightListMLP;
    std::list<std::string> weight2StrListMLP;
    std::list<cv::Mat> str2WeightListMLP;
    std::string weight2StrMLP;

    //MSG
    cMessage* trainModelEvt;
    cMessage* sendModelEvt;
};

} // namespace veins
