/**@title
 * Federated Averaging (FedAVG)
 *
 * @brief
 *
 *
 * @author Wellington Lobato (wellington@lrc.ic.unicamp.br)
 *
 */
#pragma once
#include <algorithm>
#include <vector>
#include <random>
#include <tuple>

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "../messages/FlexeMessage_m.h"
#include "../application/FlexeClient.h"

using namespace omnetpp;
using namespace veins;

namespace flexe {
class FedRANDOMRSU11p : public DemoBaseApplLayer {
public:
    ~FedRANDOMRSU11p();
    void initialize(int stage) override;

protected:
    //MSG TYPE
    enum selfMessageKinds {
        AGGREGATE_MODELS_EVT = 16,
        CLIENT_SELECTION_EVT = 17,
        SEND_GLOBAL_MODEL_EVT = 18,
        SEND_LOCAL_MODEL_EVT = 19,
    };
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;

    //VAR
    int self;
    //NED VARs
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
    std::string strategy;
    double computationRequirement;
    double roundDeadline;
    double K;
    //NED VARs

    int numSelectedClients;
    int counter;
    int localModelID;
    double lossModel;
    double accuracyModel;
    int numExamples;
    double entropy;
    char signalName[32];
    std::string outputMSG;
    std::string signalMetricName;
    std::string statisticMetricName;

    std::string clientsModels;
    std::string numExamplesModels;
    std::string selectedClients;

    std::map<int, std::string> connectedClientsMap; //(Veins-ID, SUMO-ID)
    std::map<std::string, double> datasetComputationRequirementMap; //(Dataset, Computation Requirement)
    std::map<std::string, std::string> datasetModelMap; //(Dataset, Model)
    std::map<std::string, int> datasetMetricMap; //(Dataset, Metric Index)

    std::vector<int> connectedClientsVector;
    std::vector<int> receivedModelsVector;
    std::vector<int> receivedNumberExamplesVector;

    std::vector<std::string> datasetVector;
    std::vector<std::string> modelVector;
    std::vector<std::string> computationRequirementVector;

    ModelReply modelOutput;
    GenericReply genericOutput;

    std::string address;
    grpc::ChannelArguments chArgs;
    FlexeClient* client = NULL;

    //TRACI
    TraCIScenarioManager* manager;
    TraCICommandInterface* traci;

    //MSG
    cMessage* aggregateModelsEvt = new cMessage("Aggregate received models event", AGGREGATE_MODELS_EVT);
    cMessage* clientSelectionEvt = new cMessage("Client selection mechanism event", CLIENT_SELECTION_EVT);
    cMessage* sendGlobalModelEvt = new cMessage("Send Aggregated Model Event", SEND_GLOBAL_MODEL_EVT);

    //METRICS
    simsignal_t selectClientsMetric;
    simsignal_t receivedClientsMetric;
    simsignal_t connectedClientsMetric;
    simsignal_t roundMetric;
    simsignal_t signal;
    cProperty *statisticTemplate;

    //METRICS FOR 10 DIFFERENT DATASETS
    simsignal_t lossTrainSignals[10];
    simsignal_t lossEvaluateSignals[10];
    simsignal_t accuracyTrainSignals[10];
    simsignal_t accuracyEvaluateSignals[10];
};
}
