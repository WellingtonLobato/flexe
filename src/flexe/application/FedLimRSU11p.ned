package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple FedLimRSU11p extends DemoBaseApplLayer
{
    @class(flexe::FedLimRSU11p);
    //General Parameters 
    string model = default("NONE");
    string dataset = default("MNIST");
    string scenario = default("GRID");
    int seed = default(42);
    bool nonIID = default(false);
    bool trainFlag = default(true);
    int epochs = default(1);
    int batch = default(1024);    
    int numClients = default(1);
    double percentDataset = default(100.0);
    string strategy = default("FED_AVG");
    string computationRequirement = default("900"); //in cycles/bit
    
    //GRPC Address
    string address = default("127.0.0.1:5000");
    
    //SemiSync Parameters
    double roundDeadline = default(60.0);
    double K = default(1.0); //Selection Fraction of Clients (1.0 -> 100%, 0.5 -> 50%, 0.25 -> 25%)

	@signal[connectedClients](type="long");
    @statistic[connectedClientsCount](title="connectedClients count"; source="connectedClients"; record=vector,stats; interpolationmode=none);

    @signal[selectClients](type="long");
    @statistic[selectClientsCount](title="selectClients count"; source="selectClients"; record=vector,stats; interpolationmode=none);
    
    @signal[receivedClients](type="long");
    @statistic[receivedClientsClientsCount](title="receivedClients count"; source="receivedClients"; record=vector,stats; interpolationmode=none);
    
    @signal[round](type="double");
    @statistic[roundCount](title="round count"; source="round"; record=vector,stats; interpolationmode=none);
    
    @signal[lossTrain](type="double");
    @statistic[lossTrainCount](title="lossTrain count"; source="lossTrain"; record=vector,stats; interpolationmode=none);
    
    @signal[accuracyTrain](type="double");
    @statistic[accuracyTrainCount](title="accuracyTrain count"; source="accuracyTrain"; record=vector,stats; interpolationmode=none);
    
    @signal[lossEvaluate](type="double");
    @statistic[lossEvaluateCount](title="lossEvaluate count"; source="lossEvaluate"; record=vector,stats; interpolationmode=none);
    
    @signal[accuracyEvaluate](type="double");
    @statistic[accuracyEvaluateCount](title="accuracyEvaluate count"; source="accuracyEvaluate"; record=vector,stats; interpolationmode=none);
    
}