#ifndef MLP_H
#define MLP_H

#include <vector>
#include <cmath>
#include <list>
#include <iomanip>      // std::setprecision
#include <sstream>
#include <fstream>      //Check if file exist
#include <dirent.h>
#include <sys/types.h>
#include <opencv4/opencv2/core/core_c.h>
#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/ml/ml.hpp>

using namespace std;

class MLP {
public:
    MLP(unsigned int nFeatures
            , unsigned int nClasses
            , unsigned int nLayers
            , unsigned int nHiddenLayer
            , unsigned int max_iter
            , double eps){
        m_nFeatures = nFeatures;
        m_nClasses = nClasses;
        m_nLayers = nLayers;
        m_nHiddenLayer = nHiddenLayer;
        m_max_iter = max_iter;
        m_eps = eps;
    }
    ~MLP(){}

    tuple <int, std::string> trainMLP(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName, double splitRatio);
    float testMLP(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName, std::string saveFileName, double splitRatio);
    std::list<cv::Mat> fedAVG(std::map<int, std::tuple<int, std::string>> modelsMap, std::string loadFileName);

    std::list<cv::Mat> getModelWeights(std::string loadFileName);
    std::list<std::string> convertWeightsToListStr(std::list<cv::Mat> layers);
    std::string convertWeightsToStr(std::list<cv::Mat> layers);
    std::list<cv::Mat> convertStrToWeights(std::string strLayers);

    cv::Ptr<cv::ml::TrainData> splitDatasetEqual(const cv::Ptr<cv::ml::TrainData> &dataset, double splitDatasetRatio);

    void showDataset(const cv::Ptr<cv::ml::TrainData> &dataset);
    cv::Mat oneHotEncode(const cv::Mat& labels, const int numClasses);
    std::vector<std::string> split(std::string str,std::string sep);
    bool loadImage(std::string imagePath, cv::Mat& outputImage);
    std::vector<std::string> getFilesNamesInFolder(std::string folder, std::string format);

private:
    unsigned int m_nFeatures;
    unsigned int m_nClasses;
    unsigned int m_nLayers;
    unsigned int m_nHiddenLayer;
    std::list<cv::Mat> m_weightList;
    std::ifstream fedAVGfile;
    std::fstream my_fedAVGfile;
    unsigned int m_max_iter;
    double m_eps;

    const int WIDTH_SIZE = 50;
    const int HEIGHT_SIZE = (int)(WIDTH_SIZE * sqrt(3)) / 2;
    const int IMAGE_DATA_SIZE = WIDTH_SIZE * HEIGHT_SIZE;
};

#endif // MLP_H
