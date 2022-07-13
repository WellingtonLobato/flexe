#include "veins/modules/application/traci/RandomNumberCloud.h"

using namespace veins;

Define_Module(veins::RandomNumberCloud);


RandomNumberCloud::~RandomNumberCloud() {
}

void RandomNumberCloud::onWSA(DemoServiceAdvertisment* wsa){
}

void RandomNumberCloud::onWSM(BaseFrame1609_4* frame){
}

void RandomNumberCloud::onBSM(DemoSafetyMessage* bsm){
}

void RandomNumberCloud::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case CLOUD_EVT:{
        std::cout << simTime().dbl() <<" RandomNumberCloud::handleSelfMsg - App Cloud!!! : " << msg->getKind() << endl;
        std::cout << simTime().dbl() <<" RandomNumberCloud::handleSelfMsg - App Cloud!!! : " << msg->getName() << endl;
        media = 0.0;
        modelDataStr = DemoBaseApplLayer::split(msg->getName(), ";");
        for(itModelDataStr = modelDataStr.begin(); itModelDataStr != modelDataStr.end(); itModelDataStr++){
            media = media + std::stod(*itModelDataStr);
        }
        double lenVector = modelDataStr.size();
        media = media/lenVector;
        std::cout << "media: " << media << endl;
        break;
    }
    default: {
        break;
    }
    }
}

