#pragma once

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <time.h>

using namespace omnetpp;

namespace veins {

/**
 *
 * @author Wellington Lobato
 *
 */

class VEINS_API RandomNumberApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~RandomNumberApp();

protected:
    //MSG TYPE
    enum selfMessageKinds {
        BEACON_MSG_EVT,
        MODEL_MSG_EVT,
    };
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;



    //VAR
    int self;
    int iSecret;

    //MSG
    cMessage* sendModelEvt;
};

} // namespace veins
