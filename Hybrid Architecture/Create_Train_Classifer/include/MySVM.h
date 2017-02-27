#ifndef MYSVM_H
#define MYSVM_H

// Standard Libraies
#include <iostream>
#include <vector>
#include <unordered_map>

//Project Classes
#include <Category.h>
#include <ImageGetFeature.h>


//OpenCV imports
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

class MySVM
{
    public:
        //Constructor : Params SVM_Name , Category , -ve features
        MySVM( std::unordered_map<int,Category> all_cat, string name="" , Category cat = Category() , vector < vector<float> > clutter_features_tr = vector < vector<float> >() , vector < vector<float> > clutter_features_ts = vector < vector<float> >()  );
        //This will reassign the class category to c
        void setCategory(Category& c);
        //This will set the negative features to new_negative_features, if true to train else test // default ==true
        void setNegativeFeatures(vector < vector<float> > new_negative_features , bool test_or_train = true);
        //this will add a negative feature to negative features , if true to train else test // default ==true
        void addNegativeFeature(vector<float> neg_feat , bool test_or_train = true);

        //This will prepare train and test images and extract the features for them
        void prepare_category_train_test_features( ImageGetFeature& igf , int train = -1 , int test = 0);   // Overloaded on the basis of Category.h
        void prepare_category_train_test_features( ImageGetFeature& igf ,float train = 0.9f , float test = 0.1f); // Overloaded on the basis of Category.h

        Ptr<SVM> create_train_models(bool gridSearch = false);
        void test_trained_models();
        void create_and_test_models(bool gridSearch = false);


        virtual ~MySVM();
        MySVM(const MySVM& other);



    protected:
        string svmName;
        Category category;
        std::unordered_map<int,Category> all_categories;
        vector < vector<float> > negative_features_train , negative_features_test ;
        vector < vector<float> > cat_train_features , cat_test_features;
        vector <int> train_labels , test_labels;
        Mat trainingDataMat, labelsMat, weights;

        //Classifer Model Pointers
        Ptr<SVM> svm;
        Ptr<ml::KNearest> knn;


    private:
        void _prepareVectors(ImageGetFeature& igf);
        void _prepareData();



};

#endif // MYSVM_H
