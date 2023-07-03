package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple SemiSyncRSU11p extends DemoBaseApplLayer
{
    @class(flexe::SemiSyncRSU11p);
    string cloudModuleName = default("NO_NAME");
    string modelName = default("NONE");
    string address = default("127.0.0.1:5000");
    int epochs = default(1);
    int batchSize = default(32);
    double roundDeadlineTime = default(60.0); //1 minute -> 60 //3 minutes -> 180 // 5 minutes -> 300
    bool trainFlag = default(true);

    @signal[selectID](type="int");
    @statistic[selectCount](title="selectID count"; source="selectID"; record=vector,stats; interpolationmode=none);
    
    @signal[round](type="double");
    @statistic[roundCount](title="round count"; source="round"; record=vector,stats; interpolationmode=none);
}
