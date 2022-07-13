#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class VEINS_API FederatedLearningCloud : public DemoBaseApplLayer {
public:
    ~FederatedLearningCloud();


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
};

} // namespace veins
