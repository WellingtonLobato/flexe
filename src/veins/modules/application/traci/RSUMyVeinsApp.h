#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class VEINS_API RSUMyVeinsApp : public DemoBaseApplLayer {
public:
    ~RSUMyVeinsApp();


protected:
    //MSG TYPE
    enum selfMessageKinds {
        BEACON_MSG_EVT,
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
