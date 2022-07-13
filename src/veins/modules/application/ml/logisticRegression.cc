#include "logisticRegression.h"

void LogisticRegression::trainLogisticRegression(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName){
    auto logistic_regression = cv::ml::LogisticRegression::create();
    logistic_regression->setLearningRate(m_learningRate);
    logistic_regression->setIterations(m_iterations);
    logistic_regression->setRegularization(cv::ml::LogisticRegression::REG_L2);
    logistic_regression->setTrainMethod(cv::ml::LogisticRegression::MINI_BATCH);
    logistic_regression->setMiniBatchSize(m_miniBatchSize);
    cv::Mat trainData = dataset->getTrainSamples();
    cv::Mat trainLabels = dataset->getTrainResponses();
    trainLabels.convertTo(trainLabels, CV_32F);
    logistic_regression->train(trainData, 0, trainLabels);
    logistic_regression->save(saveFileName);
}

float LogisticRegression::testLogisticRegression(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName){
    auto logistic_regression = cv::ml::LogisticRegression::load(loadFileName);
    std::vector<int32_t> predictions;
    auto error = logistic_regression->calcError(dataset, true, predictions);
    return error;
}
