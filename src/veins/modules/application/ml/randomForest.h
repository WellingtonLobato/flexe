#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include <vector>
#include <cmath>
#include <opencv4/opencv2/core/core_c.h>
#include <opencv4/opencv2/ml/ml.hpp>

using namespace std;

class randomForest {
public:    
    randomForest(unsigned int maxCategories, unsigned int maxDepth, unsigned int minSampleCount,
            bool truncatePrunedTree, bool use1SERule, bool useSurrogates, unsigned int cvFolds){
        m_maxCategories = maxCategories;
        m_maxDepth = maxDepth;
        m_minSampleCount = minSampleCount;
        m_truncatePrunedTree = truncatePrunedTree;
        m_use1SERule = use1SERule;
        m_useSurrogates = useSurrogates;
        m_cvFolds = cvFolds;
    }
    ~randomForest(){}

    void trainRandomForest(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName);
    float testRandomForest(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName);


private:
    unsigned int m_maxCategories;
    unsigned int m_maxDepth;
    unsigned int m_minSampleCount;
    bool m_truncatePrunedTree;
    bool m_use1SERule;
    bool m_useSurrogates;
    unsigned int m_cvFolds;
};

#endif // RANDOMFOREST_H
