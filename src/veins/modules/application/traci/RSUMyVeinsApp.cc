#include "veins/modules/application/traci/RSUMyVeinsApp.h"

using namespace veins;

Define_Module(veins::RSUMyVeinsApp);


RSUMyVeinsApp::~RSUMyVeinsApp() {
}

void RSUMyVeinsApp::onWSA(DemoServiceAdvertisment* wsa){
    std::cout << "RSU onWSA - Recebi?" << endl;
}

void RSUMyVeinsApp::onWSM(BaseFrame1609_4* frame){
    std::cout << "RSU onWSM - Recebi?" << endl;
}

void RSUMyVeinsApp::onBSM(DemoSafetyMessage* bsm){
    std::cout << "RSU onBSM - Recebi?" << endl;
    switch(bsm->getKind()){
    default:{
        std::cout << "onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void RSUMyVeinsApp::handleSelfMsg(cMessage* msg){
    std::cout << "RSU handleSelfMsg - Recebi?" << msg->getName() << " " << msg->getKind() << endl;
    switch (msg->getKind()) {
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

