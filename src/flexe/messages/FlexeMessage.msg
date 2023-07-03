import veins.modules.messages.DemoSafetyMessage;

packet FlexeMessage extends veins::BaseFrame1609_4 {
    int senderID = -1;
    char tensors[];
    int tensorsSize[];
    int num_examples;
    int modelVersion;
    double loss;
    double accuracy;
}
