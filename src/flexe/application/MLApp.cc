#include "MLApp.h"

using namespace veins;
using namespace flexe;

Define_Module(MLApp);

void MLApp::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        self = getParentModule()->getIndex();
        epochs = par("epochs").intValue();
        batchSize = par("batchSize").intValue();
        modelName = par("modelName").stringValue();
        address = par("address").stringValue();
        trainFlag = par("trainFlag").boolValue();
        sendTrainInterval = par("sendTrainInterval").doubleValue();
    }
    else if (stage == 1) {
        chArgs.SetMaxReceiveMessageSize(-1);
        chArgs.SetMaxSendMessageSize(-1);
        client = new FlexeClient(grpc::CreateCustomChannel(address, grpc::InsecureChannelCredentials(), chArgs));
        modelVersion = 0;
        scheduleAt(simTime().dbl()+(uniform(0.0,0.01)*self), trainModelEvt);
    }
}

MLApp::~MLApp(){
    cancelAndDelete(sendModelEvt);
    cancelAndDelete(trainModelEvt);
    delete client;
}

void MLApp::finish(){
    DemoBaseApplLayer::finish();
}

void MLApp::onBSM(DemoSafetyMessage* bsm){
    switch(bsm->getKind()){
    case SEND_BEACON_EVT:{
        break;
    }
    case SEND_CLOUD_EVT:{
        break;
    }
    default:{
        std::cout << self << " - (onBSM) The message type was not detected. " << bsm->getKind() << endl;
        break;
    }
    }
}

void MLApp::onWSM(BaseFrame1609_4* wsm){
    switch(wsm->getKind()){
        case SEND_CLOUD_EVT:{
            break;
        }
        case SEND_FED_MODEL_EVT:{
            FlexeMessage* flexe_msg = check_and_cast<FlexeMessage*>(wsm);
            client->update_model(self, trainFlag, modelName, 1, 1);
            modelVersion = flexe_msg->getModelVersion();
            trainFlag = true;
            std::cout << self << " - (onWSM) Update the local model. modelVersion: "<< modelVersion << " SEND_FED_MODEL_EVT: " << simTime().dbl() << endl;
            break;
        }
        default:{
            std::cout << self << " - (onWSM) The message type was not detected. " << wsm->getKind() << endl;
            break;
        }
    }
}

void MLApp::onWSA(DemoServiceAdvertisment* wsa){
}

void MLApp::handleSelfMsg(cMessage* msg){
    switch (msg->getKind()) {
        case SEND_BEACON_EVT:{
            DemoSafetyMessage* bsm = new DemoSafetyMessage();
            populateWSM(bsm);
            bsm->setName(std::to_string(self).c_str());
            sendDown(bsm);
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            break;
        }

        case TRAIN_MODEL_EVT:{
            modelResponse = client->fit(self, trainFlag, modelName, epochs, batchSize);
            if(trainFlag){
                loss = client->evaluate(self, trainFlag, modelName, batchSize);
            }
            trainFlag = false;
            std::cout << self << " - Loss: " << loss << " TRAIN_MODEL_EVT: " << simTime().dbl() << endl;
            scheduleAt(simTime().dbl()+uniform(0.0,0.01), sendModelEvt);
            break;
        }

        case SEND_MODEL_EVT:{
            std::cout << self << " - SEND_MODEL_EVT: " << simTime().dbl() << endl;
            strWeights = "";
            tensorSizeVector.clear();

            for(int i=0; i < modelResponse.tensors_size(); i++){
                tensorSizeVector.push_back(modelResponse.tensors(i).size());
                strWeights = strWeights + modelResponse.tensors(i);
            }

            std::vector<char> tensors_bytes(strWeights.begin(), strWeights.end());

            FlexeMessage* flexe_msg = new FlexeMessage();
            DemoBaseApplLayer::populateWSM(flexe_msg);

            flexe_msg->setTensorsArraySize(tensors_bytes.size());
            counter = 0;
            for(std::vector<char>::iterator itr = tensors_bytes.begin(); itr != tensors_bytes.end(); ++itr){
                flexe_msg->setTensors(counter, *itr);
                counter++;
            }

            flexe_msg->setTensorsSizeArraySize(modelResponse.tensors_size());
            counter = 0;
            for(std::vector<int>::iterator itr = tensorSizeVector.begin(); itr != tensorSizeVector.end(); ++itr){
                flexe_msg->setTensorsSize(counter, *itr);
                counter++;
            }

            flexe_msg->setNum_examples(modelResponse.num_examples());
            flexe_msg->setSenderID(self);
            flexe_msg->setKind(SEND_CLOUD_EVT);
            flexe_msg->setModelVersion(modelVersion);
            flexe_msg->setLoss(loss);

            // CLEAR MEMORY
            strWeights.clear();
            std::vector<char>().swap(tensors_bytes);
            tensors_bytes.clear();
            tensors_bytes.shrink_to_fit();


            std::vector<int>().swap(tensorSizeVector);
            tensorSizeVector.clear();
            tensorSizeVector.shrink_to_fit();

            modelResponse.Clear();
            // CLEAR MEMORY

            DemoBaseApplLayer::sendDelayedDown(flexe_msg, uniform(0.0,0.1));

            scheduleAt(simTime().dbl()+sendTrainInterval, trainModelEvt);
            break;
        }

        case SEND_FED_MODEL_EVT:{
            std::cout << self << " - SEND_FED_MODEL_EVT: " << simTime().dbl() << endl;
            break;
        }

        default: {
            std::cout << self << " handleSelfMsg - The message type was not detected. " << msg->getKind() << endl;
            break;
        }
    }
}

void MLApp::handlePositionUpdate(cObject* obj){
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
