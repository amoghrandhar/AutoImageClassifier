    #include <opencv2/core.hpp>
    #include <opencv2/imgproc.hpp>
    #include "opencv2/imgcodecs.hpp"
    #include <opencv2/highgui.hpp>
    #include <opencv2/ml.hpp>

    #include <MongoCaltech256.h>
    #include <ImageGetFeature.h>
    #include <MySVM.h>

    #include <unordered_map>
    #include <thread>

    using namespace cv;
    using namespace cv::ml;

    using namespace std;


    string model_layer_name = "VGG_NET_MODEL_fc7"; // _fc7 , _fc8 too MEAN_
    string imCollection = "image_set";
    unordered_map<int,Category> ic;

    // Clutter features are used to store all the negetive features // Background noise features
    vector< vector<float> > clut_train , clut_test;



    void create_mySVM(int id){

       // cout << " CATEGORY "<< id << endl;

        MySVM m_svm = MySVM(ic , "Cat_", ic[id] , clut_train , clut_test);
        ImageGetFeature igf_temp = ImageGetFeature("ImageData" , imCollection , model_layer_name);
        m_svm.prepare_category_train_test_features(igf_temp,30,25);
        m_svm.create_and_test_models(false);

    }


    int main(int, char**)
    {
        //To get the unique mongoDB drivers
        mongocxx::instance inst{};

        MongoCaltech256 mcl256 = MongoCaltech256();
        ImageGetFeature igf = ImageGetFeature("ImageData" , imCollection , model_layer_name);

        cout << " Calling Setup Caltech \n\n" ;
        mcl256.setupCaltech();
        ic = mcl256.getCategoryMap();

        cout << " Getting Clutter Caltech \n\n" ;
        Category clut = ic[mcl256.getClutterID()];
        clut.createTrainTestImages(30,25);

        //Reserving that many element space in vector as the number of images
        clut_train.reserve(clut.getTrainImages().size());
        clut_test.reserve(clut.getTestImages().size());
        // Extracting the festures of all clutter images

        cout << " Extracting Clutter Image Features \n\n" ;

        for (string image : clut.getTrainImages() ){
            clut_train.push_back(igf.getFeaturesImage(image));
        }

        for (string image : clut.getTestImages()){
            clut_test.push_back(igf.getFeaturesImage(image));
        }


        //create_mySVM(211);


        vector<std::thread> myThreads;

        //Round 1
        for(unsigned int i = 200 ; i < 256; i++){  //i < ic.size()
            auto t = std::thread(create_mySVM,i+1);
            myThreads.push_back(std::move(t));
        }

        for (auto &t : myThreads){
            if (t.joinable()) {
                t.join();
            }
        }

    }

