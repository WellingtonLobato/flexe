package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple FederatedLearningRSU11p extends DemoBaseApplLayer
{
    @class(veins::FederatedLearningRSU11p);
    string cloudModuleName = default("NO_NAME");
}
