#ifndef SVM_H
#define SVM_H

#include <vector>
#include <cmath>
#include <opencv4/opencv2/core/core_c.h>
#include <opencv4/opencv2/ml/ml.hpp>

using namespace std;

class SVM {
public:    
    SVM(){
    }
    ~SVM(){}

    void trainSVM(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName);
    float testSVM(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName);

private:
};

#endif // SVM_H
