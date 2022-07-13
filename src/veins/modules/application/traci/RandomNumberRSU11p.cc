#include "veins/modules/application/traci/RandomNumberRSU11p.h"

using namespace veins;

Define_Module(veins::RandomNumberRSU11p);


RandomNumberRSU11p::~RandomNumberRSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(data);
}

void RandomNumberRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void RandomNumberRSU11p::onWSM(BaseFrame1609_4* frame){
}

void RandomNumberRSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case BEACON_MSG_EVT:{
        //GET NUMBER
        std::cout << bsm->getSourceAddress() << " - RSU Numero Aleatorio: " <<  bsm->getModelData() << " SIM_TIME: " << simTime() << endl;
        modelData = modelData + ";" + bsm->getModelData();
        //GET NUMBER

        //First Scheduling
        if(eventoEscalonado == 0){
            eventoEscalonado = 1;
            self = getParentModule()->getIndex();
            sendCloudEvt = new cMessage("Send Data to Cloud Layer Event", SEND_CLOUD_EVT);

            cloudModuleName = par("cloudModuleName").stringValue();
            windowTime = par("windowTime").doubleValue();
            //Configure TRACI
            manager = TraCIScenarioManagerAccess().get();
            ASSERT(manager);
            traci = manager->getCommandInterface();

            if (!traci) {
                std::cout << "Cannot create screenshot: TraCI is not connected yet" << endl;
            }
            //Configure TRACI
            scheduleAt(simTime().dbl() + windowTime, sendCloudEvt);
        }
        //First Scheduling
        break;
    }
    default:{
        std::cout << "onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void RandomNumberRSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        std::cout << "" << endl;
        std::cout << modelData << endl;
        std::cout << "" << endl;
        if(cloudModuleName.compare("NO_NAME") != 0){
            data = new cMessage(modelData.c_str());
            data->setKind(CLOUD_EVT);
            DemoBaseApplLayer::sendCloud(data->dup(), cloudModuleName);
        }else{
            std::cout << "Cloud name not defined." << endl;
        }

        //scheduleAt(simTime().dbl() + windowTime, sendCloudEvt);
        modelData = "";
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

