package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple RandomNumberRSU11p extends DemoBaseApplLayer
{
    @class(veins::RandomNumberRSU11p);
    string cloudModuleName = default("NO_NAME");
    double windowTime = default(10.0);
}
