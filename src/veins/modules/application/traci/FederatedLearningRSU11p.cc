#include "veins/modules/application/traci/FederatedLearningRSU11p.h"

using namespace veins;

Define_Module(veins::FederatedLearningRSU11p);


FederatedLearningRSU11p::~FederatedLearningRSU11p() {
    cancelAndDelete(sendCloudEvt);
    cancelAndDelete(data);
}

void FederatedLearningRSU11p::onWSA(DemoServiceAdvertisment* wsa){
}

void FederatedLearningRSU11p::onWSM(BaseFrame1609_4* frame){
}

void FederatedLearningRSU11p::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case BEACON_MSG_EVT:{
        //First Scheduling
        if(eventoEscalonado == 0){
            eventoEscalonado = 1;
            self = getParentModule()->getIndex();
            sendCloudEvt = new cMessage("Send Data to Cloud Layer Event", SEND_CLOUD_EVT);
            cloudModuleName = par("cloudModuleName").stringValue();
            //Configure TRACI
            manager = TraCIScenarioManagerAccess().get();
            ASSERT(manager);
            traci = manager->getCommandInterface();
            if (!traci) {
                std::cout << "Cannot create screenshot: TraCI is not connected yet" << endl;
            }
            //Configure TRACI
            scheduleAt(simTime().dbl() + 0.1, sendCloudEvt);
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

void FederatedLearningRSU11p::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case SEND_CLOUD_EVT:{
        /*
         * CRIAR UMA MENSAGEM E MANDAR DIRETAMENTE PARA A CLOUD (DUAS OPÇÕES, COM ATRASO OU SEM ATRASO)
         */
        if(cloudModuleName.compare("NO_NAME") != 0){
            data = new cMessage("Dados para a nuvem!");
            data->setKind(CLOUD_EVT);
            DemoBaseApplLayer::sendCloud(data->dup(), cloudModuleName);
        }else{
            std::cout << "Cloud name not defined." << endl;
        }
        break;
    }
    default: {
        std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

