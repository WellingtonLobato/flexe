package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple MLRSU11p extends DemoBaseApplLayer
{
    @class(flexe::MLRSU11p);
    string cloudModuleName = default("NO_NAME");
    string dataset = default("NONE");
    string modelName = default("NONE");
    string address = default("127.0.0.1:5000");
    int epochs = default(1);
    int batchSize = default(32);
    
    @signal[selectID](type="int");
    @statistic[selectCount](title="selectID count"; source="selectID"; record=vector,stats; interpolationmode=none);
    
    @signal[round](type="int");
    @statistic[roundCount](title="round count"; source="round"; record=vector,stats; interpolationmode=none);
}