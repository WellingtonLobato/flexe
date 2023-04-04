package flexe.application;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple MLApp extends DemoBaseApplLayer
{
    @class(flexe::MLApp);
    string dataset = default("NONE");
    string modelName = default("NONE");
    string address = default("127.0.0.1:5000");
    int epochs = default(1);
    int batchSize = default(32);
    double trainInterval = default(10.0);
}