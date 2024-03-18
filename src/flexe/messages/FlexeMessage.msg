import veins.modules.messages.DemoSafetyMessage;

packet FlexeMessage extends veins::BaseFrame1609_4 {
	int senderID = -1;
    string model;
    string dataset;
    string scenario;
    int seed;
    bool nonIID;
    int idModel;
    bool trainFlag;
    int numClients;
    double percentDataset;
    int numExamples;
    int modelVersion;
    string clientSelection;
    double loss;
    double accuracy;
    double entropy;
    double computationCapability;
    double computationRequirement;
}