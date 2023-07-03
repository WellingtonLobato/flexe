/**@title
 * 
 *
 * @brief
 *
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
using flexe::ModelRequest;
using flexe::EvaluateReply;
using flexe::ModelReply;
using flexe::GenericRequest;
using flexe::GenericResponse;

class FlexeClient {
    public:
    FlexeClient(std::shared_ptr<Channel> channel)
    : stub_(Flexe::NewStub(channel)) {}

    int end(const std::string& messageGRPC){
        GenericRequest msg;
        msg.set_msg(messageGRPC);
        GenericResponse reply;
        ClientContext context;

        Status status = stub_->end(&context, msg, &reply);
        context.TryCancel();
        if(status.ok()){
            //std::cout << "End the Simulation!" << endl;
            return 1;
        }else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    double aggregate_evaluate(int idRSU, bool trainFlag, const std::string& modelName, int batchSize){
        TTRequest evaluateRequest;
        evaluateRequest.set_idvehicle(idRSU);
        evaluateRequest.set_trainflag(false);
        evaluateRequest.set_modelname(modelName);
        evaluateRequest.set_epochs(-1);
        evaluateRequest.set_batch_size(batchSize);

        ClientContext context;
        EvaluateReply reply;

        Status status = stub_->aggregate_evaluate(&context, evaluateRequest, &reply);
        if(status.ok()){
            //std::cout << "Aggregate Evaluate requested successfully!" << endl;
            return reply.loss();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }


    ModelReply aggregate_fit(ModelRequest modelGRPC){
        ClientContext context;
        ModelReply reply;

        Status status = stub_->aggregate_fit(&context, modelGRPC, &reply);
        if(status.ok()){
            //std::cout << "Aggregate Fit requested successfully!" << endl;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        return reply;
    }


    ModelReply aggregate_sync_fit(const std::string& messageGRPC){
        GenericRequest msg;
        msg.set_msg(messageGRPC);
        ClientContext context;
        ModelReply reply;
        Status status = stub_->aggregate_sync_fit(&context, msg, &reply);

        if(status.ok()){
            //std::cout << "Aggregate Semi-Async requested successfully!" << endl;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        return reply;
    }

    double server_evaluate(int idRSU, bool trainFlag, const std::string& modelName, int batchSize){
        TTRequest ServerEvaluateRequest;
        ServerEvaluateRequest.set_idvehicle(idRSU);
        ServerEvaluateRequest.set_trainflag(true);
        ServerEvaluateRequest.set_modelname(modelName);
        ServerEvaluateRequest.set_epochs(-1); //NOT USED
        ServerEvaluateRequest.set_batch_size(batchSize);
        ClientContext context;
        EvaluateReply reply;

        Status status = stub_->server_evaluate(&context, ServerEvaluateRequest, &reply);
        if(status.ok()){
            //std::cout << "Server Evaluate requested successfully!" << endl;
            return reply.loss();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return reply.loss();
        }
    }


    ModelReply initialize_parameters(int idRSU, bool trainFlag, const std::string& modelName, int epochs, int batchSize){
        TTRequest InitializeParametersRequest;
        InitializeParametersRequest.set_idvehicle(idRSU);
        InitializeParametersRequest.set_trainflag(trainFlag);
        InitializeParametersRequest.set_modelname(modelName);
        InitializeParametersRequest.set_epochs(epochs);
        InitializeParametersRequest.set_batch_size(batchSize);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->initialize_parameters(&context, InitializeParametersRequest, &reply);
        if(status.ok()){
            //std::cout << "Initialize Parameters requested successfully!" << endl;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        return reply;
    }

    int store_model(ModelRequest modelGRPC){
        ClientContext context;
        GenericResponse reply;

        Status status = stub_->store_model(&context, modelGRPC, &reply);
        if(status.ok()){
            //std::cout << "Stored Model requested successfully!" << endl;
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

class SemiSyncRSU11p : public DemoBaseApplLayer {
public:
    ~SemiSyncRSU11p();
    void initialize(int stage) override;

protected:
    //MSG TYPE
    enum selfMessageKinds {
        SEND_CLOUD_EVT = 10,
        SEND_FED_MODEL_EVT = 17,
        CLOUD_EVT = 12207,
    };
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;

    //VAR
    int firstScheduling = 0;
    int self;
    int counter;
    int epochs;
    int batchSize;
    int sourceID;
    int nextModelID;

    bool trainFlag;

    double roundDeadlineTime;
    double coverage;
    double diff;

    std::string cloudModuleName;
    std::string modelStr;
    std::string modelName;
    std::string strWeights;
    std::string vehicleIDs;

    std::vector<char> tensorsBytes;
    std::vector<int> tensorSizeVector;
    std::vector<int> receivedModels;
    std::vector<int>::iterator kt;

    std::map<int, std::tuple<std::string, std::vector<int>>> modelsMap;
    std::map<int, std::tuple<std::string, std::vector<int>>>::iterator it;

    std::map<int, std::tuple<double, double>> contactMap;

    EvaluateReply evaluateResponse;
    ModelReply modelResponse;

    std::string address;
    grpc::ChannelArguments chArgs;
    FlexeClient* client = NULL;

    //TRACI
    TraCIScenarioManager* manager;
    TraCICommandInterface* traci;

    //MSG
    cMessage* sendCloudEvt = new cMessage("Send Data to Cloud Layer Event", SEND_CLOUD_EVT);
    cMessage* sendModelEvt = new cMessage("Send Aggregated Model Event", SEND_FED_MODEL_EVT);

    //METRICS
    simsignal_t selectIDMetric;
    simsignal_t roundMetric;
};
}
