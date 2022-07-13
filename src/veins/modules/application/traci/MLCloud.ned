package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple MLCloud extends DemoBaseApplLayer
{
    @class(veins::MLCloud);
    string rsuModuleName = default("NO_NAME");
    int minNumberClients = default(2);
    //MLP Options
    int numberLayers = default(4);
    int hiddenLayer = default(50);
    string datasetTrainFile = default("NO_FILE");
    string datasetTestFile = default("NO_FILE");
    bool customRead = default(false);
    string delimiter = default(",");
    string modelFile = default("NO_FILE");
    double splitRatio = default(0.75);
    int max_iter = default(500);
    double eps = default(0.0001);
    //Dataset Options
    int headerLineCount = default(0);
    int responseStartIdx = default(-1);
    int responseEndIdx = default(-1);
}
