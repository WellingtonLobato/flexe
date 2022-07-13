#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class VEINS_API MLRSU11p : public DemoBaseApplLayer {
public:
    ~MLRSU11p();

protected:
    //MSG TYPE
    enum selfMessageKinds {
        BEACON_MSG_EVT,
        SEND_CLOUD_EVT = 10,
        SEND_FED_MODEL_EVT = 17,
        CLOUD_EVT = 12207,
    };
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onBSM(DemoSafetyMessage* bsm) override;
    void handleSelfMsg(cMessage* msg) override;

    //VAR
    int eventoEscalonado = 0;
    int self;
    int numTrainData;
    int selfTrainData;
    std::string cloudModuleName;
    std::string modelStr;

    //TRACI
    TraCIScenarioManager* manager;
    TraCICommandInterface* traci;

    //MSG
    cMessage* sendCloudEvt = new cMessage("Send Data to Cloud Layer Event", SEND_CLOUD_EVT);
    cMessage* data2Cloud = new cMessage("NULL");
};

} // namespace veins
