/**@title Machine Learning Application
 *
 * @brief
 *
 * @author Wellington Lobato (wellington@lrc.ic.unicamp.br)
 *
 */
#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "../messages/FlexeMessage_m.h"
#include "../application/FlexeClient.h"

using namespace omnetpp;
using namespace veins;

namespace flexe {

class MLApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~MLApp();

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    //MSG TYPE
    enum selfMessageKinds {
        TRAIN_LOCAL_MODEL_EVT = 16,
        CLIENT_SELECTION_EVT = 17,
        SEND_GLOBAL_MODEL_EVT = 18,
        SEND_LOCAL_MODEL_EVT = 19,
        RESOURCE_REQUEST_EVT = 20,
    };

    //VAR
    int self;
    std::string model;
    std::string dataset;
    std::string scenario;
    int seed;
    bool nonIID;
    int idModel;
    bool trainFlag;
    int epochs;
    int batch;
    int numClients;
    double percentDataset;
    double computationCapability;
    int modelVersion;

    bool speedZero;
    bool resourceFlag;

    int counter;
    double computationRequirement;
    double lossModel;
    double accuracyModel;
    int numExamples;
    double entropy;
    std::string outputMSG;
    double trainingLatency;

    ModelReply modelOutput;
    GenericReply genericOutput;

    std::string address;
    grpc::ChannelArguments chArgs;
    FlexeClient* client = NULL;

    //MSG
    cMessage* trainLocalModelEvt = new cMessage("Train the local model on the client event", TRAIN_LOCAL_MODEL_EVT);
    cMessage* sendResourceRequestEvt = new cMessage("Send Resource Request Event", RESOURCE_REQUEST_EVT);

    //METRICS
    simsignal_t trainLatencyMetric;
    simsignal_t compCapabilityMetric;
    simsignal_t lossTrainMetric;
    simsignal_t accuracyTrainMetric;
    simsignal_t lossEvaluateMetric;
    simsignal_t accuracyEvaluateMetric;
    simsignal_t lossUpdateMetric;
    simsignal_t accuracyUpdateMetric;
};
}
