#ifndef TRAINSVM_H
#define TRAINSVM_H
#include <opencv/cv.hpp>
#include <iostream>

#include <opencv2/ml.hpp>

using namespace std;

class TrainSVM
{
    public:
        TrainSVM(int id, string cat);
        virtual ~TrainSVM();


    protected:
        int cat_ID;
        string category;
        cv::ml::ParamGrid pGrid = cv::ml::ParamGrid();
        //cv::ml::SVM mySVM = cv::ml::SVM::create();




    private:
};

#endif // TRAINSVM_H
