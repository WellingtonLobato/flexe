package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple FLOERSU11p extends DemoBaseApplLayer
{
    @class(flexe::FLOERSU11p);
    string cloudModuleName = default("NO_NAME");
    string dataset = default("NONE");
    string modelName = default("NONE");
    string address = default("127.0.0.1:5000");
    int epochs = default(1);
    int batchSize = default(32);
    double roundDeadlineTime = default(10.0);
    
    @signal[selectID](type="int");
    @statistic[selectCount](title="selectID count"; source="selectID"; record=vector,stats; interpolationmode=none);
    
    @signal[round](type="double");
    @statistic[roundCount](title="round count"; source="round"; record=vector,stats; interpolationmode=none);
}