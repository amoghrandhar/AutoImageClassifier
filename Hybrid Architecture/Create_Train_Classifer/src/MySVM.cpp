    #include "MySVM.h"
    //OpenCV imports
    #include <opencv2/core.hpp>
    #include <opencv2/imgproc.hpp>
    #include "opencv2/imgcodecs.hpp"
    #include <opencv2/highgui.hpp>
    #include <opencv2/ml.hpp>

    using namespace std;
    using namespace cv;
    using namespace cv::ml;


    MySVM::MySVM(std::unordered_map<int,Category> all_cat, string name , Category c , vector < vector<float> > clutter_features_tr , vector < vector<float> > clutter_features_ts)
    {
        //ctor
        this->all_categories = all_cat;
        this->svmName = name;
        this->category = c;
        this->negative_features_train = clutter_features_tr;
        this->negative_features_test = clutter_features_ts;
    }

    //This will reassign the class category to c
    void MySVM::setCategory(Category& c){
        this->category = c;
    }

    //This will set the negative features to new_negative_features
    void MySVM::setNegativeFeatures(vector < vector<float> > new_negative_features , bool test_or_train){
        if(test_or_train) {
            this->negative_features_train = new_negative_features;
        }
        else {
            this->negative_features_test = new_negative_features;
        }
    }

    //this will add a negative feature to negative features
    void MySVM::addNegativeFeature(vector<float> neg_feat , bool test_or_train){
        if(test_or_train) {
            this->negative_features_train.push_back(neg_feat);
        }
        else {
            this->negative_features_test.push_back(neg_feat);
        }
    }


    //This will prepare train and test images and extract the features for them
    // Overloaded on the basis of Category.h
    void MySVM::prepare_category_train_test_features(ImageGetFeature& igf ,int train, int test){
        this->category.createTrainTestImages(train, test);
        _prepareVectors(igf);
    }

    //This will prepare train and test images and extract the features for them
    // Overloaded on the basis of Category.h
    void MySVM::prepare_category_train_test_features(ImageGetFeature& igf , float train , float test){
        this->category.createTrainTestImages(train, test);
        _prepareVectors(igf);
    }


    void MySVM::_prepareVectors(ImageGetFeature& imgf){
        //RESET THE CATEGORY TRAIN AND TEST VECTORS AND PREPARE THEM
        this->cat_train_features.clear();
        this->cat_test_features.clear();

        this->cat_train_features.reserve(category.getTrainImages().size());
        this->cat_test_features.reserve(category.getTestImages().size());


        // Adding 2 images from all the other classes to -ve train and 10 random to -ve test.
        vector<string> temp_test_images;
        for(auto kv : this->all_categories) {
            if(kv.first != this->category.getID() && kv.first != 257 ){
                Category c_other = kv.second ;
                string image = c_other.getAllImages()[0];
                negative_features_train.push_back(imgf.getFeaturesImage(image));
                image = c_other.getAllImages()[1];
                negative_features_train.push_back(imgf.getFeaturesImage(image));
                temp_test_images.push_back(c_other.getAllImages()[3]);
            }
        }


        //Randomizing the test list.
        ///*
        std::srand(std::time(0));
        std::random_shuffle(temp_test_images.begin(),temp_test_images.end() );
        for(int i = 0 ; i < 10 ; i++){
                negative_features_test.push_back(imgf.getFeaturesImage(temp_test_images[i]));
        }
        //*/


        // getting the features of all train images
        for (string image : category.getTrainImages()){
            cat_train_features.push_back(imgf.getFeaturesImage(image));
        }
        // getting the features of all test images
        for (string image : category.getTestImages()){
            cat_test_features.push_back(imgf.getFeaturesImage(image));
        }

        this->train_labels.clear();
        this->test_labels.clear();

        this->train_labels = vector<int>(cat_train_features.size() , 1);
        this->test_labels = vector<int>(cat_test_features.size() , 1);

        //NOW ADDING THE NEGATIVE FEATURES TO TRAIN AND TEST FEATURES
        this->cat_train_features.insert(cat_train_features.end() , this->negative_features_train.begin(),this->negative_features_train.end());
        this->cat_test_features.insert(cat_test_features.end() , this->negative_features_test.begin(),this->negative_features_test.end());

        this->train_labels.insert(train_labels.end() , negative_features_train.size() , -1);
        this->test_labels.insert(test_labels.end() , negative_features_test.size() , -1);
        //This will prepare data variables to used by openCV ML models
        _prepareData();
    }

    //This will prepare data variables to used by openCV ML models
    void MySVM::_prepareData(){
        //Number Of Rows And Columns
        int r = cat_train_features.size();
        int c = cat_train_features[0].size();

        // Creating a 2d Float array from vector of vector of floats of feature
        float** tr_data = new float*[r];
        for(int i = 0; i < r; ++i){
            tr_data[i] = new float[c];
        }
        for(unsigned int i = 0 ; i < cat_train_features.size() ; i++){
            std::copy(cat_train_features[i].begin(),cat_train_features[i].end(),tr_data[i]);
        }

        //Loading The training matrix and labels matrix
        trainingDataMat = Mat(cat_train_features.size() , cat_train_features[0].size() , CV_32FC1 , tr_data ).clone();
        labelsMat = Mat(train_labels.size(), 1, CV_32SC1, &train_labels[0]).clone();

        // Setting Class weights - > On the basis of numbers of classes to normalize
        vector<float> w;
        w.push_back(cat_train_features.size() - negative_features_train.size());
        w.push_back(negative_features_train.size());
        this->weights = Mat(w).clone();
    }

    Ptr<SVM> MySVM::create_train_models(bool gridSearch){
        // Train the SVM
        this->svm = SVM::create();
        // Whether to use the pre set parameter or find by running grid search
        if(!gridSearch){
            svm->setType(SVM::C_SVC);
            svm->setC(0.1);
            svm->setGamma(1.0000000000000001e-05);
            svm->setKernel(SVM::RBF);
            //svm->setC(0.001);
            //svm->setGamma(50);
            //svm->setKernel(SVM::LINEAR);
            svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, (int)1e7, 1e-6));
        }

        svm->setClassWeights(weights);

        if(gridSearch){
            _InputArray tr_data1(trainingDataMat);
            _InputArray lab(labelsMat);
            Ptr<TrainData> trainData_ptr = TrainData::create(tr_data1 , ROW_SAMPLE , lab);
            svm->trainAuto(trainData_ptr);
        } else {
            svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
        }

        //Train k-NN
        this->knn = ml::KNearest::create();
        knn->setIsClassifier(true);
        knn->setAlgorithmType(KNearest::Types::BRUTE_FORCE);
        knn->setDefaultK(5);
        knn->train(trainingDataMat, ROW_SAMPLE, labelsMat);

        svm->save("cal_svm/" + this->svmName + std::to_string(this->category.getID()));
        knn->save("cal_knn/" + this->svmName + std::to_string(this->category.getID()));

        //Mat wei = svm->getClassWeights();
        /*
        cout << " Weights : " << endl ;
        for(int i = 0 ; i < wei.rows ; i++){
            const float* v = wei.ptr<float>(i);
            for(int j = 0 ; j < wei.cols ; ++j){
               cout << (int) v[j] << "  ";
            }
        }

        cout << endl ;
        */


        //Mat sv = svm->getSupportVectors();
        /*
        for (int i = 0; i < sv.rows; ++i)
        {
            const float* v = sv.ptr<float>(i);
            for(int j = 0 ; j < sv.cols ; ++j){
                cout << (int) v[j] << "  ";
            }

            cout <<  endl; //(int) v[0] << "  "<< (int) v[4098] <<
        }
        */

        //cout << "Support Vectors :::  \n\n Rows : " << sv.rows << " Cols :  "<< sv.cols <<endl ;
        return svm;
    }

    void MySVM::test_trained_models(){
         //cout << " \n\n Testing \n\n " ;
        int correct_classified_svn = 0 , truePositives_Cassifeid_svm=0;
        int correct_classified_knn = 0 , truePositives_Cassifeid_knn=0;

        //Testing
        Mat dum = Mat(0,0,CV_32F);

        for( unsigned int i = 0 ; i < cat_test_features.size() ; i++ ){
            //Mat test(1,cat_test_features[0].size(),CV_32FC1 , &cat_test_features[i][0] );
            Mat test1(cat_test_features[i]);
            Mat test = test1.t();

            //dum.push_back(test);

        /*
                Mat pca_projection_ts_128;
                _InputArray ts_data_pca(test);
                PCA pca1(ts_data_pca ,Mat(),CV_PCA_DATA_AS_COL, 128);
                pca1.project(ts_data_pca,pca_projection_ts_128);
                cout << "\n\n PCA -- TEST : ROWS  -:- " << pca_projection_ts_128.rows << " : COLOUMS -:- " << pca_projection_ts_128.cols << endl << endl ;

        */
            auto response = svm->predict(test);
            //cout << " SVM PREDICTION :: " << response << " Actual Label : " << test_labels[i] <<  endl;
            //auto response1 = bayes->predict(test);
            //cout << " BAYES PREDICTION :: " << response1 << " Actual Label : " << test_labels[i] <<  endl;

            auto response1 = knn->predict(test);
            //cout << " KNN PREDICTION :: " << response1 << " Actual Label : " << test_labels[i] <<  endl;
            if((int)response == test_labels[i]){
                correct_classified_svn++;
                if(test_labels[i] == 1){
                    truePositives_Cassifeid_svm++;
                }
            }
            if ((int) response1 == test_labels[i]){
                correct_classified_knn++;
                if(test_labels[i] == 1){
                    truePositives_Cassifeid_knn++;
                }
            }

        }


        cout << "Category ID : " << this->category.getID() << " : \tCat Name : " << this->category.getCategory()
             << " :\tTotal number of test images = : " << cat_test_features.size()
             << " :\tCorrectly classified images " << "SVM :" << correct_classified_svn
             << " :\tCorrectly TP " << "SVM :" << truePositives_Cassifeid_svm
             << " :\tCorrectly TP " << "KNN :" << truePositives_Cassifeid_knn
             << " :\tKNN : " << correct_classified_knn << endl
             << " \n##\n" ;
    /*
        Mat matResults(0,0,CV_32F);
        knn->findNearest(dum,knn->getDefaultK(),matResults);

        cout << " \n TEST "<< dum.rows << "  " << dum.cols << endl;
        cout << " \n knn "<< matResults << endl;

    */
    }

    void MySVM::create_and_test_models(bool gridSearch){
        this->create_train_models(gridSearch);
        this->test_trained_models();
    }

    MySVM::~MySVM()
    {
        //dtor
    }

    MySVM::MySVM(const MySVM& other)
    {
        this->category = other.category;
        this->svmName = other.svmName;
        this->negative_features_train = other.negative_features_train;
        this->negative_features_test = other.negative_features_test;
        this->cat_test_features = other.cat_test_features;
        this->cat_train_features = other.cat_train_features;
        this->train_labels = other.train_labels;
        this->test_labels = other.test_labels;
    }
