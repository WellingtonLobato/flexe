#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

#include <vector>
#include <cmath>
#include <opencv4/opencv2/core/core_c.h>
#include <opencv4/opencv2/ml/ml.hpp>

using namespace std;

class LogisticRegression {
public:    
    LogisticRegression(double learningRate, unsigned int iterations, unsigned int miniBatchSize){
        m_learningRate = learningRate;
        m_iterations = iterations;
        m_miniBatchSize = miniBatchSize;
    }
    ~LogisticRegression(){}

    void trainLogisticRegression(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName);
    float testLogisticRegression(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName);


private:
    double m_learningRate;
    unsigned int m_iterations;
    unsigned int m_miniBatchSize;
};

#endif // LOGISTIC_REGRESSION_H
