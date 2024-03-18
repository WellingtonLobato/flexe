import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network MultiModelScenario extends Scenario{
    submodules:
        rsu[5]: RSU {
            @display("p=200,140;i=veins/sign/yellowdiamond;is=vs");
        }
}
