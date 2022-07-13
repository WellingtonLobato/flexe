package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple SignApp extends DemoBaseApplLayer
{
    @class(veins::SignApp);
    double windowTime = default(10.0);
    int numRounds = default(5);
    //MLP Options
    int numberLayers = default(4);
    int hiddenLayer = default(50);
    string datasetTrainFile = default("NO_FILE");
    string datasetTestFile = default("NO_FILE");
    bool customRead = default(false);
    string delimiter = default(",");
    string modelFile = default("NO_FILE");
    double splitRatio = default(0.75);
    double splitClientRatio = default(0.10);
    bool nonIID = default(false);
    int max_iter = default(500);
    double eps = default(0.0001);
    double trainInterval = default(10.0);
    //Dataset Options
    int headerLineCount = default(0);
    int responseStartIdx = default(-1);
    int responseEndIdx = default(-1);
}
