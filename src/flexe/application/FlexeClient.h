#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h>

#include "../proto/flexe.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using flexe::Flexe;
using flexe::TTRequest;
using flexe::AggregationRequest;
using flexe::GenericRequest;
using flexe::GenericReply;
using flexe::ModelReply;

class FlexeClient {
    public:
    FlexeClient(std::shared_ptr<Channel> channel)
    : stub_(Flexe::NewStub(channel)) {}

    // Server Functions
    std::tuple<int, double, double, double, std::string> center_fit( //GenericRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest centerFitRequest;
        centerFitRequest.set_identity(idEntity+1);
        centerFitRequest.set_model(model);
        centerFitRequest.set_dataset(dataset);
        centerFitRequest.set_scenario(scenario);
        centerFitRequest.set_seed(seed);
        centerFitRequest.set_noniid(nonIID);
        centerFitRequest.set_idmodel(idModel+1);
        centerFitRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->center_fit(&context, centerFitRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Center Fit requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> center_evaluate( //GenericRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest centerEvaluateRequest;
        centerEvaluateRequest.set_identity(idEntity+1);
        centerEvaluateRequest.set_model(model);
        centerEvaluateRequest.set_dataset(dataset);
        centerEvaluateRequest.set_scenario(scenario);
        centerEvaluateRequest.set_seed(seed);
        centerEvaluateRequest.set_noniid(nonIID);
        centerEvaluateRequest.set_idmodel(idModel+1);
        centerEvaluateRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->center_evaluate(&context, centerEvaluateRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Center Evaluate requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, std::string> set_strategy( //GenericRequest -> GenericReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest setStrategyRequest;
        setStrategyRequest.set_identity(idEntity+1);
        setStrategyRequest.set_model(model);
        setStrategyRequest.set_dataset(dataset);
        setStrategyRequest.set_scenario(scenario);
        setStrategyRequest.set_seed(seed);
        setStrategyRequest.set_noniid(nonIID);
        setStrategyRequest.set_idmodel(idModel+1);
        setStrategyRequest.set_msg(msg);

        ClientContext context;
        GenericReply reply;

        Status status = stub_->set_strategy(&context, setStrategyRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Set Strategy requested successfully!" << endl;
            return {reply.reply(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, "ERROR"};
        }
    }


    std::tuple<int, std::string> get_strategy( //GenericRequest -> GenericReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest getStrategyRequest;
        getStrategyRequest.set_identity(idEntity+1);
        getStrategyRequest.set_model(model);
        getStrategyRequest.set_dataset(dataset);
        getStrategyRequest.set_scenario(scenario);
        getStrategyRequest.set_seed(seed);
        getStrategyRequest.set_noniid(nonIID);
        getStrategyRequest.set_idmodel(idModel+1);
        getStrategyRequest.set_msg(msg);

        ClientContext context;
        GenericReply reply;

        Status status = stub_->get_strategy(&context, getStrategyRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Get Strategy requested successfully!" << endl;
            return {reply.reply(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, "ERROR"};
        }
    }

    std::tuple<int, std::string> end( //GenericRequest -> GenericReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest endRequest;
        endRequest.set_identity(idEntity+1);
        endRequest.set_model(model);
        endRequest.set_dataset(dataset);
        endRequest.set_scenario(scenario);
        endRequest.set_seed(seed);
        endRequest.set_noniid(nonIID);
        endRequest.set_idmodel(idModel+1);
        endRequest.set_msg(msg);

        ClientContext context;
        GenericReply reply;

        Status status = stub_->end(&context, endRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - End requested successfully!" << endl;
            return {reply.reply(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> initialize_parameters( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest InitializeParametersRequest;
        InitializeParametersRequest.set_identity(idEntity+1);
        InitializeParametersRequest.set_model(model);
        InitializeParametersRequest.set_dataset(dataset);
        InitializeParametersRequest.set_scenario(scenario);
        InitializeParametersRequest.set_seed(seed);
        InitializeParametersRequest.set_noniid(nonIID);
        InitializeParametersRequest.set_idmodel(idModel+1);
        InitializeParametersRequest.set_trainflag(trainFlag);
        InitializeParametersRequest.set_epochs(epochs);
        InitializeParametersRequest.set_batch(batch);
        InitializeParametersRequest.set_numclients(numClients);
        InitializeParametersRequest.set_percentdataset(percentDataset);
        InitializeParametersRequest.set_models(models);
        InitializeParametersRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->initialize_parameters(&context, InitializeParametersRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Initialize Parameters requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> aggregate_sync_fit( //AggregationRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            int numClients,
            const std::string& numExamples,
            const std::string& models,
            const std::string& strategy,
            const std::string& msg
            ){
        AggregationRequest aggregateFitRequest;
        aggregateFitRequest.set_identity(idEntity+1);
        aggregateFitRequest.set_model(model);
        aggregateFitRequest.set_dataset(dataset);
        aggregateFitRequest.set_scenario(scenario);
        aggregateFitRequest.set_seed(seed);
        aggregateFitRequest.set_noniid(nonIID);
        aggregateFitRequest.set_idmodel(idModel+1);
        aggregateFitRequest.set_numclients(numClients);
        aggregateFitRequest.set_numexamples(numExamples);
        aggregateFitRequest.set_models(models);
        aggregateFitRequest.set_strategy(strategy);
        aggregateFitRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->aggregate_sync_fit(&context, aggregateFitRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Aggregate Fit requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> aggregate_async_fit( //AggregationRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            int numClients,
            const std::string& numExamples,
            const std::string& models,
            const std::string& strategy,
            const std::string& msg
            ){
        AggregationRequest aggregateAsyncFitRequest;
        aggregateAsyncFitRequest.set_identity(idEntity+1);
        aggregateAsyncFitRequest.set_model(model);
        aggregateAsyncFitRequest.set_dataset(dataset);
        aggregateAsyncFitRequest.set_scenario(scenario);
        aggregateAsyncFitRequest.set_seed(seed);
        aggregateAsyncFitRequest.set_noniid(nonIID);
        aggregateAsyncFitRequest.set_idmodel(idModel+1);
        aggregateAsyncFitRequest.set_numclients(numClients);
        aggregateAsyncFitRequest.set_numexamples(numExamples);
        aggregateAsyncFitRequest.set_models(models);
        aggregateAsyncFitRequest.set_strategy(strategy);
        aggregateAsyncFitRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->aggregate_async_fit(&context, aggregateAsyncFitRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Aggregate Async. Fit requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> aggregate_evaluate( //AggregationRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            int numClients,
            const std::string& numExamples,
            const std::string& models,
            const std::string& strategy,
            const std::string& msg
            ){

        AggregationRequest aggregateEvaluateRequest;
        aggregateEvaluateRequest.set_identity(idEntity+1);
        aggregateEvaluateRequest.set_model(model);
        aggregateEvaluateRequest.set_dataset(dataset);
        aggregateEvaluateRequest.set_scenario(scenario);
        aggregateEvaluateRequest.set_seed(seed);
        aggregateEvaluateRequest.set_noniid(nonIID);
        aggregateEvaluateRequest.set_idmodel(idModel+1);
        aggregateEvaluateRequest.set_numclients(numClients);
        aggregateEvaluateRequest.set_numexamples(numExamples);
        aggregateEvaluateRequest.set_models(models);
        aggregateEvaluateRequest.set_strategy(strategy);
        aggregateEvaluateRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->aggregate_evaluate(&context, aggregateEvaluateRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Aggregate Evaluate requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    // Client Functions
    std::tuple<int, double, double, double, std::string> fit( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest fitRequest;
        fitRequest.set_identity(idEntity+1);
        fitRequest.set_model(model);
        fitRequest.set_dataset(dataset);
        fitRequest.set_scenario(scenario);
        fitRequest.set_seed(seed);
        fitRequest.set_noniid(nonIID);
        fitRequest.set_idmodel(idModel+1);
        fitRequest.set_trainflag(trainFlag);
        fitRequest.set_epochs(epochs);
        fitRequest.set_batch(batch);
        fitRequest.set_numclients(numClients);
        fitRequest.set_percentdataset(percentDataset);
        fitRequest.set_models(models);
        fitRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->fit(&context, fitRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Fit requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> evaluate( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest evaluateRequest;
        evaluateRequest.set_identity(idEntity+1);
        evaluateRequest.set_model(model);
        evaluateRequest.set_dataset(dataset);
        evaluateRequest.set_scenario(scenario);
        evaluateRequest.set_seed(seed);
        evaluateRequest.set_noniid(nonIID);
        evaluateRequest.set_idmodel(idModel+1);
        evaluateRequest.set_trainflag(trainFlag);
        evaluateRequest.set_epochs(epochs);
        evaluateRequest.set_batch(batch);
        evaluateRequest.set_numclients(numClients);
        evaluateRequest.set_percentdataset(percentDataset);
        evaluateRequest.set_models(models);
        evaluateRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->evaluate(&context, evaluateRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Evaluation requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> update_model( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest updateRequest;
        updateRequest.set_identity(idEntity+1);
        updateRequest.set_model(model);
        updateRequest.set_dataset(dataset);
        updateRequest.set_scenario(scenario);
        updateRequest.set_seed(seed);
        updateRequest.set_noniid(nonIID);
        updateRequest.set_idmodel(idModel+1);
        updateRequest.set_trainflag(trainFlag);
        updateRequest.set_epochs(epochs);
        updateRequest.set_batch(batch);
        updateRequest.set_numclients(numClients);
        updateRequest.set_percentdataset(percentDataset);
        updateRequest.set_models(models);
        updateRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->update_model(&context, updateRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Update requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> get_information( //GenericRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest getInfoRequest;
        getInfoRequest.set_identity(idEntity+1);
        getInfoRequest.set_model(model);
        getInfoRequest.set_dataset(dataset);
        getInfoRequest.set_scenario(scenario);
        getInfoRequest.set_seed(seed);
        getInfoRequest.set_noniid(nonIID);
        getInfoRequest.set_idmodel(idModel+1);
        getInfoRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->get_information(&context, getInfoRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Get Info requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::tuple<int, double, double, double, std::string> set_information( //GenericRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            const std::string& msg
            ){
        GenericRequest setInfoRequest;
        setInfoRequest.set_identity(idEntity+1);
        setInfoRequest.set_model(model);
        setInfoRequest.set_dataset(dataset);
        setInfoRequest.set_scenario(scenario);
        setInfoRequest.set_seed(seed);
        setInfoRequest.set_noniid(nonIID);
        setInfoRequest.set_idmodel(idModel+1);
        setInfoRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->set_information(&context, setInfoRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Set Info requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> aggregate_client( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest aggregateClientRequest;
        aggregateClientRequest.set_identity(idEntity+1);
        aggregateClientRequest.set_model(model);
        aggregateClientRequest.set_dataset(dataset);
        aggregateClientRequest.set_scenario(scenario);
        aggregateClientRequest.set_seed(seed);
        aggregateClientRequest.set_noniid(nonIID);
        aggregateClientRequest.set_idmodel(idModel+1);
        aggregateClientRequest.set_trainflag(trainFlag);
        aggregateClientRequest.set_epochs(epochs);
        aggregateClientRequest.set_batch(batch);
        aggregateClientRequest.set_numclients(numClients);
        aggregateClientRequest.set_percentdataset(percentDataset);
        aggregateClientRequest.set_models(models);
        aggregateClientRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->aggregate_client(&context, aggregateClientRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Aggregate Client requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> fit_all( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest fitAllRequest;
        fitAllRequest.set_identity(idEntity+1);
        fitAllRequest.set_model(model);
        fitAllRequest.set_dataset(dataset);
        fitAllRequest.set_scenario(scenario);
        fitAllRequest.set_seed(seed);
        fitAllRequest.set_noniid(nonIID);
        fitAllRequest.set_idmodel(idModel+1);
        fitAllRequest.set_trainflag(trainFlag);
        fitAllRequest.set_epochs(epochs);
        fitAllRequest.set_batch(batch);
        fitAllRequest.set_numclients(numClients);
        fitAllRequest.set_percentdataset(percentDataset);
        fitAllRequest.set_models(models);
        fitAllRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->fit_all(&context, fitAllRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Fit All requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> evaluate_all( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest evaluateAllRequest;
        evaluateAllRequest.set_identity(idEntity+1);
        evaluateAllRequest.set_model(model);
        evaluateAllRequest.set_dataset(dataset);
        evaluateAllRequest.set_scenario(scenario);
        evaluateAllRequest.set_seed(seed);
        evaluateAllRequest.set_noniid(nonIID);
        evaluateAllRequest.set_idmodel(idModel+1);
        evaluateAllRequest.set_trainflag(trainFlag);
        evaluateAllRequest.set_epochs(epochs);
        evaluateAllRequest.set_batch(batch);
        evaluateAllRequest.set_numclients(numClients);
        evaluateAllRequest.set_percentdataset(percentDataset);
        evaluateAllRequest.set_models(models);
        evaluateAllRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->evaluate_all(&context, evaluateAllRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Evaluate All requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }


    std::tuple<int, double, double, double, std::string> update_all( //TTRequest -> ModelReply
            int idEntity,
            const std::string& model,
            const std::string& dataset,
            const std::string& scenario,
            int seed,
            bool nonIID,
            int idModel,
            bool trainFlag,
            int epochs,
            int batch,
            int numClients,
            double percentDataset,
            const std::string& models,
            const std::string& msg
            ){
        TTRequest updateAllRequest;
        updateAllRequest.set_identity(idEntity+1);
        updateAllRequest.set_model(model);
        updateAllRequest.set_dataset(dataset);
        updateAllRequest.set_scenario(scenario);
        updateAllRequest.set_seed(seed);
        updateAllRequest.set_noniid(nonIID);
        updateAllRequest.set_idmodel(idModel+1);
        updateAllRequest.set_trainflag(trainFlag);
        updateAllRequest.set_epochs(epochs);
        updateAllRequest.set_batch(batch);
        updateAllRequest.set_numclients(numClients);
        updateAllRequest.set_percentdataset(percentDataset);
        updateAllRequest.set_models(models);
        updateAllRequest.set_msg(msg);

        ClientContext context;
        ModelReply reply;

        Status status = stub_->update_all(&context, updateAllRequest, &reply);
        if(status.ok()){
            std::cout << idEntity << " - Update All requested successfully!" << endl;
            return {reply.numexamples(), reply.entropy(), reply.loss(), reply.accuracy(), reply.msg()};
        } else {
            std::cout << status.error_code() << " - " << status.error_message() << std::endl;
            return {-1, -1.0, -1.0, -1.0, "ERROR"};
        }
    }

    std::vector<std::string> splitSTR(const std::string &s, char delim) {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    private:
        std::unique_ptr<Flexe::Stub> stub_;
};
