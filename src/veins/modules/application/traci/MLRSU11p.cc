#include "veins/modules/application/traci/MLRSU11p.h"

using namespace veins;

Define_Module(veins::MLRSU11p);


MLRSU11p::~MLRSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(data2Cloud);
}

void MLRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void MLRSU11p::onWSM(BaseFrame1609_4* frame){
}

void MLRSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_CLOUD_EVT:{
        //First Scheduling
        if(eventoEscalonado == 0){
            eventoEscalonado = 1;
            self = getParentModule()->getIndex();
            cloudModuleName = par("cloudModuleName").stringValue();
            //Configure TRACI
            manager = TraCIScenarioManagerAccess().get();
            ASSERT(manager);
            traci = manager->getCommandInterface();
            if (!traci) {
                std::cout << "Cannot create screenshot: TraCI is not connected yet" << endl;
            }
            //Configure TRACI
        }
        //First Scheduling

        modelStr = bsm->getModelData();
        numTrainData = bsm->getNumTrainData();
        selfTrainData = bsm->getSourceAddress();

        if(!sendCloudEvt->isScheduled()){
            scheduleAt(simTime().dbl() + 0.01, sendCloudEvt);
        }
        break;
    }
    default:{
        std::cout << "RUS onBSM - The message type was not detected." << bsm->getKind() << endl;
        break;
    }
    }
}

void MLRSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        if(cloudModuleName.compare("NO_NAME") != 0){
            std::string modelMsgAll = modelStr+"$"+std::to_string(numTrainData)+"$"+std::to_string(selfTrainData);
            data2Cloud->setName(modelMsgAll.c_str());
            data2Cloud->setKind(CLOUD_EVT);
            DemoBaseApplLayer::sendCloud(data2Cloud->dup(), cloudModuleName);
        }else{
            std::cout << "Cloud name not defined." << endl;
        }
        break;
    }
    case CLOUD_EVT:{
        std::cout << "RSU: RECEBI ALGO DA NUVEM!!" << endl;
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        DemoBaseApplLayer::populateWSM(bsm);
        bsm->setKind(SEND_FED_MODEL_EVT);
        DemoBaseApplLayer::sendDelayedDown(bsm, uniform(0.0,0.1));
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

