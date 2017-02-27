/*
#define feature_size 500
#define kmeans_retry_count 1

//Mongo DB -> BSON
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

//Mongo DB -> MongoDB
#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/instance.hpp>
#include <mongocxx/v_noabi/mongocxx/uri.hpp>
#include <mongocxx/v_noabi/mongocxx/options/update.hpp>

    #include <iostream>
    #include <opencv2/core.hpp>
    #include <opencv2/imgproc.hpp>
    #include "opencv2/imgcodecs.hpp"
    #include <opencv2/highgui.hpp>
    #include <opencv2/ml.hpp>
    #include <opencv2/xfeatures2d/nonfree.hpp>
    #include <opencv2/xfeatures2d.hpp>
    #include <vector>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace cv::xfeatures2d;

//MongoDB
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};
    string databaseName = "ImageData";
    string collectionName =  "image_set";
    string modelname = "SIFT_features";
    mongocxx::database dbMongo = conn[databaseName];
    mongocxx::options::update upsert;



//SIFT Feature Extractor
cv::Ptr<SIFT> sift = xfeatures2d::SIFT::create();
cv::Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");

//Storing Unclustered Features that are extracted from all the images
Mat featuresUnclustered;
// BOW KMeansTrainer
//the number of bags
int dictionarySize=feature_size;
//define Term Criteria
TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
//retries number
int retries=kmeans_retry_count;
//necessary flags
int flags=KMEANS_PP_CENTERS;
BOWKMeansTrainer bowTrainer(dictionarySize, tc, retries, flags);
BOWImgDescriptorExtractor bowDE(sift, matcher);

Mat dictionary;


void addtoUnclusterData(string file){
    Mat image_mat  = imread(file,0); // we get the grayscale component
    InputArray img = InputArray(image_mat);

    //-- Step 1: Detect the keypoints:
    std::vector<KeyPoint> keypoints;
    sift->detect( img, keypoints);
    //Getting the desciptors
    Mat descriptors;
    sift->compute( img, keypoints, descriptors );

    //put the all feature descriptors in a single Mat object
    featuresUnclustered.push_back(descriptors);

}

void prepareBagOfWords(){
    dictionary=bowTrainer.cluster(featuresUnclustered);
    bowDE.setVocabulary(dictionary);
}

void extractFeatures(string image){

    Mat image_mat = imread(image,0); // we get the grayscale component
    InputArray img = InputArray(image_mat);

    //-- Step 1: Detect the keypoints:
    std::vector<KeyPoint> keypoints;
    sift->detect( img, keypoints);
    //Getting the desciptors
    Mat descriptors;
    bowDE.compute( img, keypoints, descriptors );
	//extracting histogram in the form of bow for each image

    std::vector<float> array;
    if (descriptors.isContinuous()) {
      array.assign((float*)descriptors.datastart, (float*)descriptors.dataend);
    } else {
      for (int i = 0; i < descriptors.rows; ++i) {
        array.insert(array.end(), (float*)descriptors.ptr<uchar>(i), (float*)descriptors.ptr<uchar>(i)+descriptors.cols);
      }
    }
    float* a = &array[0];


	 bsoncxx::builder::stream::document filter_builder, update_builder;
     filter_builder << "_id" << image;

     update_builder << "$set"
                        << open_document
                        << modelname << open_document
                        << "extraction_date" << bsoncxx::types::b_date(std::chrono::system_clock::now())
                        << "feature_height" << 1
                        << "feature_width " << 1
                        << "number_of_channels" << 1
                        << "features_array_size" << feature_size
                        << "feature_array_data"
                            << open_array <<
                            [&](bsoncxx::builder::stream::array_context<> arr){
                                for (int d = 0; d < feature_size; d++)
                                arr << a[d];
                            } // Note that we don't invoke the lambda
                            << close_array
                        << close_document
                        << close_document;
    auto result_of_imageSet = dbMongo[collectionName].update_one(filter_builder.view(),update_builder.view(),upsert);

}

int main(int, char**)
{
    cout << "SIFT Feature Extractor - Mongo Connected!" << endl;

    upsert = mongocxx::options::update();
    upsert.upsert(true);


    bool forceUpdate = false; // If set to true it will update the features even if already present


    auto myDb = conn[databaseName];
    bsoncxx::builder::stream::document search_builder_images;
    search_builder_images.clear();

    // Re-Update the features if forceUpdate is set to true.
    if(!forceUpdate){
    		search_builder_images << modelname << open_document << "$exists" << false << close_document;
    }



	// Extract Features of only those images whole features aren't present
    for (bsoncxx::document::view doc : myDb[collectionName].find(search_builder_images.view()) ) {
                    bsoncxx::document::element myE = doc["_id"];
                    addtoUnclusterData( myE.get_utf8().value.to_string());
    }

    prepareBagOfWords();

    	// Extract Features of only those images whole features aren't present
    for (bsoncxx::document::view doc : myDb[collectionName].find(search_builder_images.view()) ) {
                    bsoncxx::document::element myE = doc["_id"];
                    extractFeatures( myE.get_utf8().value.to_string());
                    cout << endl;
    }


    cout << "DONE";

    return 0;
}
*/
