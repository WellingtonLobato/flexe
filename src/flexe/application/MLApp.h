/**@title Machine Learning Application
 *
 * @brief
 *
 * @author Wellington Lobato (wellington@lrc.ic.unicamp.br)
 *
 */
#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "../messages/FlexeMessage_m.h"

#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h>

#include "../proto/flexe.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using flexe::Flexe;
using flexe::TTRequest;
using flexe::EvaluateReply;
using flexe::ModelReply;
using flexe::GenericResponse;

class FlexeClient {
    public:
    FlexeClient(std::shared_ptr<Channel> channel)
    : stub_(Flexe::NewStub(channel)) {}

    double evaluate(int idVehicle, bool trainFlag, const std::string& modelName, int batchSize){
        TTRequest evaluateRequest;
        evaluateRequest.set_idvehicle(idVehicle+1);
        evaluateRequest.set_trainflag(false);
        evaluateRequest.set_modelname(modelName);
        evaluateRequest.set_batch_size(batchSize);

        ClientContext context;
        EvaluateReply reply;

        Status status = stub_->evaluate(&context, evaluateRequest, &reply);
        if(status.ok()){
            //std::cout << "Evaluation requested successfully!" << endl;
            return reply.loss();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    ModelReply fit(int idVehicle, bool trainFlag, const std::string& modelName, int epochs, int batchSize){
        TTRequest fitRequest;
        fitRequest.set_idvehicle(idVehicle+1);
        fitRequest.set_trainflag(trainFlag);
        fitRequest.set_modelname(modelName);
        fitRequest.set_epochs(epochs);
        fitRequest.set_batch_size(batchSize);
        ClientContext context;
        ModelReply reply;

        Status status = stub_->fit(&context, fitRequest, &reply);
        if(status.ok()){
            //std::cout << "Training requested successfully!" << endl;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        return reply;
    }

    int update_model(int idVehicle, bool trainFlag, const std::string& modelName, int epochs, int batchSize){
        TTRequest fitRequest;
        fitRequest.set_idvehicle(idVehicle+1);
        fitRequest.set_trainflag(false);
        fitRequest.set_modelname(modelName);
        fitRequest.set_epochs(epochs);
        fitRequest.set_batch_size(batchSize);
        GenericResponse reply;
        ClientContext context;

        Status status = stub_->update_model(&context, fitRequest, &reply);
        if(status.ok()){
            //std::cout << "Update the model requested successfully!" << endl;
            return 1;
        }else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    private:
        std::unique_ptr<Flexe::Stub> stub_;
};

using namespace omnetpp;
using namespace veins;

namespace flexe {

class MLApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~MLApp();

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    //MSG TYPE
    enum selfMessageKinds {
        TRAIN_MODEL_EVT = 7,
        SEND_MODEL_EVT,
        SEND_CLOUD_EVT = 10,
        SEND_FED_MODEL_EVT = 17,
    };

    //VAR
    int self;
    int epochs;
    int batchSize;
    int counter;
    int modelVersion;

    bool trainFlag;

    double sendTrainInterval;
    double loss;

    std::string modelName;
    std::string strWeights;

    ModelReply modelResponse;

    std::vector<int> tensorSizeVector;

    std::string address;
    grpc::ChannelArguments chArgs;
    FlexeClient* client = NULL;

    //MSG
    cMessage* trainModelEvt = new cMessage("Train Model Event", TRAIN_MODEL_EVT);
    cMessage* sendModelEvt = new cMessage("Send Model to RSU Event", SEND_MODEL_EVT);

};
}
