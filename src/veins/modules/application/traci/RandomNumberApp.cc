#include "veins/modules/application/traci/RandomNumberApp.h"

using namespace veins;

Define_Module(veins::RandomNumberApp);

void RandomNumberApp::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        self = getParentModule()->getIndex();
        sendModelEvt = new cMessage("Send Model to Cloud Layer Event", MODEL_MSG_EVT);
        /* initialize random seed: */
        std::srand(time(NULL));
    }
    else if (stage == 1) {
        scheduleAt(simTime().dbl() + beaconInterval, sendModelEvt);
    }
}

RandomNumberApp::~RandomNumberApp() {
    cancelAndDelete(sendModelEvt);
}

void RandomNumberApp::finish(){
    DemoBaseApplLayer::finish();
}

void RandomNumberApp::onBSM(DemoSafetyMessage* bsm){
}

void RandomNumberApp::onWSM(BaseFrame1609_4* wsm){
}

void RandomNumberApp::onWSA(DemoServiceAdvertisment* wsa){
}

void RandomNumberApp::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case MODEL_MSG_EVT:{
        iSecret = rand() % 10 + 1;
        std::string iSecretStr = std::to_string(iSecret);
        std::cout << self << " - Numero Aleatorio: " << iSecret << " SIM_TIME: " << simTime() << endl;
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        DemoBaseApplLayer::populateWSM(bsm);
        bsm->setModelData(iSecretStr.c_str());
        bsm->setKind(BEACON_MSG_EVT);
        bsm->setSourceAddress(self);
        DemoBaseApplLayer::sendDelayedDown(bsm, uniform(0.0,1));

        //scheduleAt(simTime().dbl() + beaconInterval, sendModelEvt);
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

void RandomNumberApp::handlePositionUpdate(cObject* obj){
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
