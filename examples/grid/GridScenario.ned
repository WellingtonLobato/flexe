import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network GridScenario extends Scenario
{
    @display("bgb=916,628");
    submodules:
        rsu[1]: RSU {
            @display("p=79.631996,297.04;i=veins/sign/yellowdiamond;is=vs");
        }
        cloud[1]: RSU {
            @display("p=752.08,548.576;i=veins/sign/yellowdiamond;is=vs");
        }
}
