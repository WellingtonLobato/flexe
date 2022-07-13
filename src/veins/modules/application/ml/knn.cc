#include "knn.h"

void KNN::trainKnn(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName){
    auto k_nearest = cv::ml::KNearest::create();
    k_nearest->setDefaultK(m_defaultK);
    k_nearest->setIsClassifier(m_isClassifier);
    k_nearest->train(dataset);
    k_nearest->save(saveFileName);
}

float KNN::testKnn(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName){
    auto k_nearest = cv::ml::KNearest::load(loadFileName);
    std::vector<int32_t> predictions;
    auto error = k_nearest->calcError(dataset, true, predictions);
    return error;
}
