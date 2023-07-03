package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple MLApp extends DemoBaseApplLayer
{
    @class(flexe::MLApp);
    string modelName = default("NONE");
    string address = default("127.0.0.1:5000");
    int epochs = default(1);
    int batchSize = default(32);
    double sendTrainInterval = default(10.0);
    bool trainFlag = default(true);
}
