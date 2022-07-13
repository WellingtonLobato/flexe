#include "svm.h"

void SVM::trainSVM(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName){
    auto svm = cv::ml::SVM::create();

    svm->setKernel(cv::ml::SVM::LINEAR); //cv::ml::SVM::RBF, cv::ml::SVM::SIGMOID, cv::ml::SVM::POLY
    svm->setType(cv::ml::SVM::C_SVC);

    svm->trainAuto(dataset);

    svm->save(saveFileName);
}

float SVM::testSVM(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName){
    auto svm = cv::ml::SVM::load(loadFileName);
    std::vector<int32_t> predictions;
    auto error = svm->calcError(dataset, true, predictions);
    return error;
}
