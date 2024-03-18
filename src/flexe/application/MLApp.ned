package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple MLApp extends DemoBaseApplLayer
{
    @class(flexe::MLApp);
    //General Parameters
    int epochs = default(1);
    int batch = default(1024);
    
    //GRPC Address
    string address = default("127.0.0.1:5000");

	//Local Training Latency Parameters
    double computationCapability @unit(GHz) = default(5GHz); //in GHz       
    bool speedZero = default(false);
    
    
    @signal[compCapability](type="double");
    @statistic[compCapability](title="compCapability count"; source="compCapability"; record=vector,stats; interpolationmode=none);
    
    @signal[trainLatency](type="double");
    @statistic[trainLatencyCount](title="trainLatency count"; source="trainLatency"; record=vector,stats; interpolationmode=none);
    
    @signal[lossTrain](type="double");
    @statistic[lossTrainCount](title="lossTrain count"; source="lossTrain"; record=vector,stats; interpolationmode=none);
    
    @signal[accuracyTrain](type="double");
    @statistic[accuracyTrainCount](title="accuracyTrain count"; source="accuracyTrain"; record=vector,stats; interpolationmode=none);
    
    @signal[lossEvaluate](type="double");
    @statistic[lossEvaluateCount](title="lossEvaluate count"; source="lossEvaluate"; record=vector,stats; interpolationmode=none);
    
    @signal[accuracyEvaluate](type="double");
    @statistic[accuracyEvaluateCount](title="accuracyEvaluate count"; source="accuracyEvaluate"; record=vector,stats; interpolationmode=none);

    @signal[lossUpdate](type="double");
    @statistic[lossUpdateCount](title="lossUpdate count"; source="lossUpdate"; record=vector,stats; interpolationmode=none);
    
    @signal[accuracyUpdate](type="double");
    @statistic[accuracyUpdateCount](title="accuracyUpdate count"; source="accuracyUpdate"; record=vector,stats; interpolationmode=none);
}