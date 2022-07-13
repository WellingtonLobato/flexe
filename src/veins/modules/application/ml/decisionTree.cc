#include "decisionTree.h"

void decisionTree::trainDecisionTree(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName){
    auto decision_tree = cv::ml::DTrees::create();
    decision_tree->setMaxCategories(m_maxCategories);
    decision_tree->setMaxDepth(m_maxDepth);
    decision_tree->setMinSampleCount(m_minSampleCount);
    decision_tree->setTruncatePrunedTree(m_truncatePrunedTree);
    decision_tree->setUse1SERule(m_use1SERule);
    decision_tree->setUseSurrogates(m_useSurrogates);
    decision_tree->setCVFolds(m_cvFolds);
    decision_tree->train(dataset);
    decision_tree->save(saveFileName);
}

float decisionTree::testDecisionTree(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName){
    auto decision_tree = cv::ml::DTrees::load(loadFileName);
    std::vector<int32_t> predictions;
    auto error = decision_tree->calcError(dataset, true, predictions);
    return error;
}
