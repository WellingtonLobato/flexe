#include "veins/modules/application/traci/FederatedLearningCloud.h"

using namespace veins;

Define_Module(veins::FederatedLearningCloud);


FederatedLearningCloud::~FederatedLearningCloud() {
}

void FederatedLearningCloud::onWSA(DemoServiceAdvertisment* wsa){
}

void FederatedLearningCloud::onWSM(BaseFrame1609_4* frame){
}

void FederatedLearningCloud::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    default:{
        break;
    }
    }
}

void FederatedLearningCloud::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
    case CLOUD_EVT:{
        std::cout << simTime().dbl() <<" FederatedLearningCloud::handleSelfMsg - App Cloud!!! : " << msg->getKind() << endl;
        /*
        img =  cv::imread(image_path, cv::IMREAD_COLOR);

        auto imagem = cv::imread(filename);
        cv::imshow("Imagem Lena", imagem);
        cv::waitKey(1);
        */
        break;
    }
    default: {
        //std::cout << "handleSelfMsg - The message type was not detected." << endl;
        break;
    }
    }
}

