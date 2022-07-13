#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/ml/mlp.h"

namespace veins {

class VEINS_API MLCloud : public DemoBaseApplLayer {
public:
    ~MLCloud();


protected:
    //MSG TYPE
    enum selfMessageKinds {
        BEACON_MSG_EVT,
        CLOUD_EVT = 12207,
    };
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onBSM(DemoSafetyMessage* bsm) override;
    void handleSelfMsg(cMessage* msg) override;

    //VAR
    int eventoEscalonado = 0;
    int self;
    //MLP|FL VAR
    int numberLayers;
    int hiddenLayer;
    int numTrainData;
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

    std::map<int, std::tuple<int, std::string>> modelsMap;

    cv::Ptr<cv::ml::TrainData> dataset;
    cv::Ptr<cv::ml::TrainData> datasetTest;
    std::list<cv::Mat> FedAVGWeightList;

    //MSG
    cMessage* data2RSU = new cMessage("NULL");;
};

} // namespace veins
