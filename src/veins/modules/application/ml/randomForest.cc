#include "randomForest.h"

void randomForest::trainRandomForest(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName){
    auto random_forest = cv::ml::RTrees::create();

    random_forest->setMaxCategories(m_maxCategories);
    random_forest->setMaxDepth(m_maxDepth);
    random_forest->setMinSampleCount(m_minSampleCount);
    random_forest->setTruncatePrunedTree(m_truncatePrunedTree);
    random_forest->setUse1SERule(m_use1SERule);
    random_forest->setUseSurrogates(m_useSurrogates);
    random_forest->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 5000, 1e-8));
    random_forest->setCVFolds(m_cvFolds);

    random_forest->train(dataset);

    random_forest->save(saveFileName);
}

float randomForest::testRandomForest(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName){
    auto random_forest = cv::ml::RTrees::load(loadFileName);
    std::vector<int32_t> predictions;
    auto error = random_forest->calcError(dataset, true, predictions);
    return error;
}
