#ifndef KNN_H
#define KNN_H

#include <vector>
#include <cmath>
#include <opencv4/opencv2/core/core_c.h>
#include <opencv4/opencv2/ml/ml.hpp>

using namespace std;

class KNN {
public:    
    KNN(unsigned int defaultK, bool isClassifier){
        m_defaultK = defaultK;
        m_isClassifier = isClassifier;
    }
    ~KNN(){}

    void trainKnn(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName);
    float testKnn(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName);


private:
    unsigned int m_defaultK;
    bool m_isClassifier;
};

#endif // KNN_H
