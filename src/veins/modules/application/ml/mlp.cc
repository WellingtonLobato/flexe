#include "mlp.h"

cv::Mat MLP::oneHotEncode(const cv::Mat& labels, const int numClasses){
    cv::Mat output(labels.rows, numClasses, CV_32F, 0.0f);
    const int* labelsPtr = labels.ptr<int>(0);
    for (int i=0; i<labels.rows; i++){
        float* outputPtr = output.ptr<float>(i);
        int id = labelsPtr[i];
        outputPtr[id] = 1.f;
    }
    return output;
}


std::vector<std::string> MLP::getFilesNamesInFolder(std::string folder, std::string format){
    std::vector<std::string> names;
    std::string fileName;
    DIR *dr;
    struct dirent *en;
    dr = opendir(folder.c_str());

    if(dr){
       while((en = readdir(dr)) != NULL){
          fileName = en->d_name;
          if(fileName.find(format) != std::string::npos){
              names.push_back(en->d_name);
          }
       }
       closedir(dr);
    }

    return names;
}


bool MLP::loadImage(std::string imagePath, cv::Mat& outputImage){
    // Load image in grayscale
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    cv::Mat temp;

    // Check for invalid input
    if (image.empty()) {
        cout << "Could not open or find the image" << std::endl;
        return false;
    }

    cv::Size size(WIDTH_SIZE, HEIGHT_SIZE);
    cv::resize(image, temp, size, 0, 0, cv::INTER_AREA);

    temp.convertTo(outputImage, CV_32FC1, 1.0/255.0);
    return true;
}

std::list<cv::Mat> MLP::fedAVG(std::map<int, std::tuple<int, std::string>> modelsMap, std::string loadFileName){
    int contador = 0;
    double numExampleTotal = 0.0;
    double numExample = 0.0;
    std::string weightStrMLP;
    cv::Mat jtMat;
    std::list<cv::Mat> weightList;
    std::vector<std::tuple<int, std::string>> weightVector;
    /*
     * INIT FED_AVG
     */
    for(std::map<int, std::tuple<int, std::string>>::iterator it = modelsMap.begin(); it != modelsMap.end(); it++){
        //std::cout << "Node Model: " << it->first << endl;
        numExample = std::get<0>(it->second);
        numExampleTotal = numExampleTotal + numExample;

        weightStrMLP = std::get<1>(it->second);
        weightVector.push_back(std::make_tuple(numExample, weightStrMLP));
    }
    numExample = std::get<0>(weightVector[0]);
    weightStrMLP = std::get<1>(weightVector[0]);
    weightList = convertStrToWeights(weightStrMLP);
    /*
     * INIT FED_AVG
     */

    /*
     * COMPUTE FED_AVG
     */
    for(std::list<cv::Mat>::iterator jt = weightList.begin(); jt != weightList.end(); jt++){
        *jt = *jt * numExample;
    }
    for(std::vector<std::tuple<int, std::string>>::iterator it = weightVector.begin(); it != weightVector.end(); it++){
        if(contador == 0){
            contador++;
            continue;
        }
        double numExampleVector = std::get<0>(*it);
        std::string weightStrMLPVector = std::get<1>(*it);
        std::list<cv::Mat> weightListVector = convertStrToWeights(weightStrMLPVector);

        std::list<cv::Mat>::iterator jt = weightList.begin();
        std::list<cv::Mat>::iterator kt = weightListVector.begin();
        for(; jt != weightList.end() && kt != weightListVector.end(); jt++, kt++){
            cv::addWeighted(*jt, 1.0, *kt, numExampleVector, 0.0, *jt);
        }
        contador++;
    }
    for(std::list<cv::Mat>::iterator jt = weightList.begin(); jt != weightList.end(); jt++){
        *jt = *jt / numExampleTotal;
    }
    /*
     * COMPUTE FED_AVG
     */

    /*
     * SAVE WEIGHTS IN FILE
     */
    std::ifstream infile(loadFileName);
    std::string line;
    std::vector<std::string> lines;
    int contadorPular = 0;

    while (std::getline(infile, line)){
        std::istringstream iss(line);
        std::string::size_type pos = 0;

        if(contadorPular != 0){
            contadorPular--;
            continue;
        }

        if(line.find("input_scale:") != std::string::npos){
            contadorPular = 0;
            lines.push_back(line);
            for(int i = 0; i < weightList.front().rows; i++){
                for(int j = 0; j < weightList.front().cols; j++){
                    std::cout.setf(std::ios::fixed, std:: ios::floatfield);
                    std::cout.precision(16);

                    std::ostringstream streamObj;
                    streamObj << std::setprecision(16);
                    streamObj << std::scientific;

                    streamObj << "      - " << weightList.front().at<double>(i,j);
                    lines.push_back(streamObj.str());
                    contadorPular++;
                }
            }
        }else if(line.find("weights:") != std::string::npos){
            contadorPular = 0;
            lines.push_back(line);
            for(std::list<cv::Mat>::iterator jt = std::next(weightList.begin()); jt != weightList.end(); jt++){
                jtMat = *jt;
                lines.push_back("      -");
                contadorPular++;

                for(int i = 0; i < jtMat.rows; i++){
                    for(int j = 0; j < jtMat.cols; j++){
                        std::cout.setf(std::ios::fixed, std:: ios::floatfield);
                        std::cout.precision(16);

                        std::ostringstream streamObj;
                        streamObj << std::setprecision(16);
                        streamObj << std::scientific;

                        streamObj << "         - " << jtMat.at<double>(i,j);
                        lines.push_back(streamObj.str());
                        contadorPular++;
                    }
                }
            }
        }else{
            lines.push_back(line);
        }
    }
    infile.close();
    std::fstream file(loadFileName, std::ios::out | std::ios::trunc);
    for(const auto& i : lines) {
        file << i << std::endl;
    }
    file.close();
    /*
     * SAVE WEIGHTS IN FILE
     */
    return weightList;
}

std::vector<std::string> MLP::split(std::string str,std::string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

std::list<cv::Mat> MLP::getModelWeights(std::string loadFileName){
    auto mlp = cv::ml::ANN_MLP::load(loadFileName);
    cv::Mat modelLayerSize = mlp->getLayerSizes();

    cv::Mat weightsMLP;
    for(int i = 0; i < modelLayerSize.rows; i++){
        weightsMLP = mlp->getWeights(i);
        m_weightList.push_back(weightsMLP);
    }
    return m_weightList;
}

std::list<std::string> MLP::convertWeightsToListStr(std::list<cv::Mat> layers){
    std::list<std::string> convertOutput;
    std::string lineLayerStr;
    for(std::list<cv::Mat>::iterator it = layers.begin(); it != layers.end(); ++it){
        cv::Mat layer = *it;
        lineLayerStr = "";
        for(int i = 0; i < layer.rows; i++){
            for(int j = 0; j < layer.cols; j++){
                lineLayerStr = lineLayerStr + std::to_string(layer.at<double>(i,j)) + ",";
            }
            lineLayerStr = lineLayerStr.substr(0, lineLayerStr.size()-1);
            lineLayerStr = lineLayerStr + ";";
        }
        convertOutput.push_back(lineLayerStr);
    }
    return convertOutput;
}

std::string MLP::convertWeightsToStr(std::list<cv::Mat> layers){
    std::list<std::string> convertOutput;
    std::string lineLayerStr;
    std::string convertOutputStr = "";
    for(std::list<cv::Mat>::iterator it = layers.begin(); it != layers.end(); ++it){
        cv::Mat layer = *it;
        lineLayerStr = "";
        for(int i = 0; i < layer.rows; i++){
            for(int j = 0; j < layer.cols; j++){
                lineLayerStr = lineLayerStr + std::to_string(layer.at<double>(i,j)) + ",";
            }
            lineLayerStr = lineLayerStr.substr(0, lineLayerStr.size()-1);
            lineLayerStr = lineLayerStr + ";";
        }
        convertOutput.push_back(lineLayerStr);
    }
    for(std::list<std::string>::iterator it = convertOutput.begin(); it != convertOutput.end(); it++){
        convertOutputStr = convertOutputStr + *it + "|";
    }
    return convertOutputStr;
}

std::list<cv::Mat> MLP::convertStrToWeights(std::string strLayers){
    std::list<cv::Mat> convertOutput;
    std::vector<std::vector<double>> vecStr2Weights;
    std::vector<double> vecValues;

    std::vector<std::string> layerSplit = MLP::split(strLayers, "|");
    int count, i, j, linhas, colunas = -1;
    for(std::vector<std::string>::iterator it = layerSplit.begin(); it != layerSplit.end(); ++it){
        std::vector<std::string> lineSplit = MLP::split(*it, ";");
        linhas = lineSplit.size();
        count = 0;
        for(std::vector<std::string>::iterator jt = lineSplit.begin(); jt != lineSplit.end(); ++jt){
            std::vector<std::string> elementSplit = MLP::split(*jt, ",");
            colunas = elementSplit.size();
            //CREATE MAT
            vecValues.clear();

            if(count == 0){
                i = 0;
                j = 0;
                count++;
            }else{
                i++;
            }
            //CREATE MAT
            for(std::vector<std::string>::iterator kt = elementSplit.begin(); kt != elementSplit.end(); ++kt){
                vecValues.push_back(std::stod(*kt));
                j++;
            }
            vecStr2Weights.push_back(vecValues);
        }
        //ADD IN LIST
        cv::Mat str2Weights(vecStr2Weights.size(), vecStr2Weights.at(0).size(), 6);
        for(int i=0; i<str2Weights.rows; ++i){
            for(int j=0; j<str2Weights.cols; ++j){
                str2Weights.at<double>(i, j) = vecStr2Weights.at(i).at(j);
            }
        }
        convertOutput.push_back(str2Weights);
        vecStr2Weights.clear();
    }
    return convertOutput;
}

cv::Ptr<cv::ml::TrainData> MLP::splitDatasetEqual(const cv::Ptr<cv::ml::TrainData> &dataset, double splitDatasetRatio){
    if(splitDatasetRatio < 1.0){
        int numRows = dataset->getSamples().rows * splitDatasetRatio;
        int rowsPerClass;
        bool equals = false;
        std::list<cv::Mat> listOutput;
        cv::Mat linhaDataset;
        cv::Mat dst;
        cv::Mat inputData = dataset->getSamples();
        cv::Mat outputData = dataset->getResponses();
        rowsPerClass = numRows/dataset->getResponses().cols;
        std::cout << "MLP::splitDatasetEqual: " << numRows << " M_classes: " << dataset->getResponses().cols << " rowsPerClass:" << rowsPerClass << endl;

        if(numRows < dataset->getResponses().cols){
            numRows = dataset->getResponses().cols;
        }

        //cout << "Input Data = "  << inputData << endl;
        //cout << "Output Data = " << outputData << endl;

        for(int i=0; i < inputData.rows; i++){
            std::cout << "inputData: " << inputData.row(i) << endl;
            std::cout << "outputData: " << outputData.row(i) << endl;
            break;
        }

        for(int i = 0; i < outputData.rows; i++){
            linhaDataset = outputData.row(i);
            equals = false;
            for(std::list<cv::Mat>::iterator it = listOutput.begin(); it!=listOutput.end(); ++it){
                cv::compare(*it, linhaDataset, dst, cv::CMP_EQ);
                int numNonZero = cv::countNonZero(dst);
                if(numNonZero == dst.cols){
                    equals = true;
                    break;
                }
            }
            if(!equals){
                listOutput.push_back(linhaDataset);
            }
        }
        std::cout << "Num de Categorias: " << linhaDataset.size() << endl;
        for(std::list<cv::Mat>::iterator it = listOutput.begin(); it!=listOutput.end(); ++it){
            std::cout << "Categorias: " << *it << endl;
        }
        return dataset;
    }
    return dataset;
}

void MLP::showDataset(const cv::Ptr<cv::ml::TrainData> &dataset){
    cv::Mat inputData = dataset->getSamples();
    cout << "Input Data = "  << inputData << endl;

    cv::Mat outputData = dataset->getResponses();
    cout << "Output Data = " << outputData << endl;
}

tuple <int, std::string> MLP::trainMLP(const cv::Ptr<cv::ml::TrainData> &dataset, std::string saveFileName, double splitRatio){
    dataset->setTrainTestSplitRatio(splitRatio, true);
    dataset->shuffleTrainTest();

    cv::Mat_<int> layers(m_nLayers, 1);
    layers(0) = m_nFeatures;             // input
    for(int i = 1; i < m_nLayers-1; i++){
        layers(i) = m_nHiddenLayer;    // hidden
    }
    layers(m_nLayers - 1) = m_nClasses; // output, 1 pin per class.

    auto ann = cv::ml::ANN_MLP::create();
    ann->setLayerSizes(layers);
    ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 0.0, 0.0); //[-1|1]
    ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, m_max_iter, m_eps));
    ann->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.0000001, 0.0001);

    std::vector<std::string> fileNameSplit = MLP::split(saveFileName, "/");

    fileNameSplit[fileNameSplit.size()-1] = "model_FedAVG.txt";
    std::string saveFileFedAVG = "/";

    for(std::vector<std::string>::iterator it = fileNameSplit.begin(); it != fileNameSplit.end(); ++it){
        saveFileFedAVG = saveFileFedAVG + *it + "/";
    }
    saveFileFedAVG = saveFileFedAVG.substr(0, saveFileFedAVG.size()-1);

    ann->train(dataset);
    ann->save(saveFileName);

    /*
     * Check if saveFileFedAVG exist already
     */
    fedAVGfile.open(saveFileFedAVG);
    if(!fedAVGfile){
        ann->save(saveFileFedAVG);
    }
    /*
     * Check if saveFileFedAVG exist already
     */
    std::list<cv::Mat> weightListMLP;
    std::string weight2StrMLP;

    weightListMLP = MLP::getModelWeights(saveFileName);
    weight2StrMLP = MLP::convertWeightsToStr(weightListMLP);
    return std::make_tuple(dataset->getNTrainSamples(), weight2StrMLP);
}

float MLP::testMLP(const cv::Ptr<cv::ml::TrainData> &dataset, std::string loadFileName, std::string saveFileName, double splitRatio){
    int numClasses = dataset->getResponses().cols;
    int conf_matrix[numClasses][numClasses] = {0};
    double true_positive[numClasses];
    double false_positive[numClasses];
    double false_negative[numClasses];
    double recall[numClasses];
    double precision[numClasses];
    std::memset(conf_matrix, 0, sizeof(conf_matrix));
    dataset->setTrainTestSplitRatio(splitRatio, true);

    auto mlp = cv::ml::ANN_MLP::load(loadFileName);

    auto testSamples = dataset->getTestSamples();
    auto testResponses = dataset->getTestResponses();

    cv::Mat result;
    mlp->predict(testSamples, result);
    double error;

    error = 0.0;
    for(int i=0; i<result.rows; ++i){
        double minVal, maxVal;
        int minIdx, maxIdx;
        cv::minMaxIdx(result.row(i).t(), &minVal, &maxVal, &minIdx, &maxIdx);
        int prediction = maxIdx;

        cv::minMaxIdx(testResponses.row(i).t(), &minVal, &maxVal, &minIdx, &maxIdx);
        int testResponse = maxIdx;
        //std::cout << prediction << ";" << testResponse << endl;
        conf_matrix[testResponse][prediction] = conf_matrix[testResponse][prediction] + 1;

        if(prediction != testResponse){
            error += 1.0;
        }
    }
    double errorRate = error / testSamples.rows;
    double accuracy = 1.0 - errorRate;

    std::cout << "Error = " << error << std::endl;
    std::cout << "errorRate = " << errorRate << std::endl;
    std::cout << "Accuracy = " << accuracy << "\n" << std::endl;
    for (int i = 0; i < numClasses; i++){
        false_negative[i] = 0;
        for (int j = 0; j < numClasses; j++){
           //std::cout << conf_matrix[i][j] << " ";
           if(i == j){
               true_positive[i] = conf_matrix[i][j];
           }else{
               false_negative[i] = false_negative[i] + conf_matrix[i][j];
           }
        }
        //std::cout << std::endl;
    }

    for (int j = 0; j < numClasses; j++){
        false_positive[j] = 0;
        for (int i = 0; i < numClasses; i++){
           if(i != j){
               false_positive[j] = false_positive[j] + conf_matrix[i][j];
           }
        }
    }

    for(int i = 0; i < numClasses; i++){
        if(true_positive[i] == 0){
            recall[i]=0;
            precision[i]=0;
        }else{
            recall[i] = true_positive[i]/(true_positive[i]+false_positive[i]);
            precision[i] = true_positive[i]/(true_positive[i]+false_negative[i]);
        }
    }

    std::string outputFile = saveFileName.substr(0, saveFileName.size()-3)+"dat";

    ofstream output(outputFile, std::ios::app);
    output << numClasses << ";" << testSamples.rows << ";" << error << ";" << errorRate << ";" << accuracy;
    for(int i = 0; i < numClasses; i++){
        output << ";" << recall[i] << ";" << precision[i];
    }
    output << endl;
    output.close();

    return error*100;
}
