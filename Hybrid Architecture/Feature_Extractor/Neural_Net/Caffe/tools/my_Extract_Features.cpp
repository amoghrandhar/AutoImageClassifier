#include <string>
#include <vector>

#include "boost/algorithm/string.hpp"
#include <boost/utility/string_ref.hpp>
#include "google/protobuf/text_format.h"

#include <caffe/caffe.hpp>
#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/net.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hpp"
#include "caffe/util/io.hpp"

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV

//Mongo DB -> BSON
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

//Mongo DB -> MongoDB
#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/instance.hpp>
#include <mongocxx/v_noabi/mongocxx/uri.hpp>
#include <mongocxx/v_noabi/mongocxx/options/update.hpp>

//shared pointer
#include <memory>

//For My Ease
using namespace std;


using caffe::Blob;
using caffe::Caffe;
using caffe::Datum;
using caffe::Net;
using namespace caffe;  // NOLINT(build/namespaces)
using std::string;
namespace db = caffe::db;

//For MONGO-DB
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};


template<typename Dtype>
int feature_extraction_pipeline(int argc, char** argv);




class ExtractFeature{

    public:
	// Constructor
        ExtractFeature(string caffeModelName , string caffeModel , string ProtoFile , string modelLayersToUse , string databaseName , bool create_ifNotPresent , string meanProtoFile = ""  );
	// Returns the name of column which is used in mongoDB to hold the features by that model & layer        
	vector<string> getMongoFeatureModelNames();
	/**
	* This is the main method
	* It returns a vector containing features for that image file from that collection dataset in database.
	*/
        void getFeatures(string fileName, string collectionName);

    private:
        void WrapInputLayer(std::vector<cv::Mat>* input_channels);
        void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);
        void SetMean(const string& mean_file);

    private:
    //This is to store all variables
        boost::shared_ptr<Net<float> > feature_extraction_net;
        std::vector<std::string> blob_names;

        cv::Size input_geometry_;
        cv::Mat mean_;

        mongocxx::database dbMongo;
        mongocxx::options::update upsert;

    public:
        string caffeModelName;
        string caffeModel;
        string protoFile ;
        string modelLayersUsed ;
        string meanProtoFile;
        size_t num_features ; // No. of layers to extract features from
        int num_channels_; // Color Channels
};

ExtractFeature::ExtractFeature(string ModelName , string Model , string ProtoFile , string modelLayersToUse , string databaseName , bool create_ifNotPresent ,string meanProtoFile){

        this->caffeModelName = ModelName;
        this->caffeModel = Model;

        // Expected prototxt contains at least one data layer such as
        //  the layer data_layer_name and one feature blob such as the
        //  fc7 top blob to extract features.
        /*

   layers {
     name: "data_layer_name"
     type: DATA
     data_param {
       source: "/path/to/your/images/to/extract/feature/images_leveldb"
       mean_file: "/path/to/your/image_mean.binaryproto"
       batch_size: 128
       crop_size: 227
       mirror: false
     }
     top: "data_blob_name"
     top: "label_blob_name"
   }
   layers {
     name: "drop7"
     type: DROPOUT
     dropout_param {
       dropout_ratio: 0.5
     }
     bottom: "fc7"
     top: "fc7"
   }

        */

    this->protoFile = ProtoFile;


    this->modelLayersUsed = modelLayersToUse;

    this->dbMongo = conn[databaseName];
    this->upsert = mongocxx::options::update();
    upsert.upsert(create_ifNotPresent);

    this->meanProtoFile = meanProtoFile;



  feature_extraction_net.reset(new Net<float>(protoFile, caffe::TEST));
  feature_extraction_net->CopyTrainedLayersFrom(caffeModel);

  boost::split(blob_names, modelLayersUsed, boost::is_any_of(",")); // So if modelLayers to user are fc7,fc8 
								//--> this will create a vector containing fc7 at v[0] and fc8 at v[1]
  num_features = blob_names.size();


  //Checking if that layer actually exists in the network or not
  for (size_t i = 0; i < num_features; i++) {
    CHECK(feature_extraction_net->has_blob(blob_names[i])) << "Unknown feature blob name " << blob_names[i] << " in the network " << modelLayersUsed;
  }

    Blob<float>* input_layer = feature_extraction_net->input_blobs()[0];
    num_channels_ = input_layer->channels();
    input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

    CHECK(num_channels_ == 3 || num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
    if(!meanProtoFile.empty()) SetMean(meanProtoFile);

}

void ExtractFeature::SetMean(const string& mean_file) {
  BlobProto blob_proto;
  ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

  /* Convert from BlobProto to Blob<float> */
  Blob<float> mean_blob;
  mean_blob.FromProto(blob_proto);
  CHECK_EQ(mean_blob.channels(), num_channels_)
    << "Number of channels of mean file doesn't match input layer.";

  /* The format of the mean file is planar 32-bit float BGR or grayscale. */
  std::vector<cv::Mat> channels;
  float* data = mean_blob.mutable_cpu_data();
      for (int i = 0; i < num_channels_; ++i) {
        /* Extract an individual channel. */
        cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
        channels.push_back(channel);
        data += mean_blob.height() * mean_blob.width();
      }

  /* Merge the separate channels into a single image. */
  cv::Mat mean;
  cv::merge(channels, mean);

  /* Compute the global mean pixel value and create a mean image
   * filled with this value. */
  cv::Scalar channel_mean = cv::mean(mean);
  mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
  }

vector<string> ExtractFeature::getMongoFeatureModelNames(){
    vector<string> modelNames = vector<string>();
    for (int i = 0; i < blob_names.size() ; i++){
        modelNames.push_back(caffeModelName + "_" + blob_names[i]);
    }

    return modelNames;
}

void ExtractFeature::getFeatures(string fileName , string collectionName){

    cv::Mat img = cv::imread(fileName, -1);
    CHECK(!img.empty()) << "Unable to decode image " << fileName;

    Blob<float>* input_layer = feature_extraction_net->input_blobs()[0];

    //Here at Reshape : BatchSize , Color-Chanels , Geometry Height And Width
    input_layer->Reshape(1, num_channels_, input_geometry_.height, input_geometry_.width);
    feature_extraction_net->Reshape();

    std::vector<cv::Mat> input_channels;
    WrapInputLayer(&input_channels);

    Preprocess(img, &input_channels);
    feature_extraction_net->Forward();

  for (int i = 0; i < blob_names.size() ; i++){ // i = 0 means for layer v[0]

    bsoncxx::builder::stream::document filter_builder, update_builder;
    filter_builder << "_id" << fileName;

  const boost::shared_ptr<Blob<float> > feature_blob = feature_extraction_net->blob_by_name(blob_names[i]);

  int batch_size = feature_blob->num(); // Its 1 (done at preprocess)
  int dim_features = feature_blob->count() / batch_size;
  const float* feature_blob_data;

  for (int n = 0; n < batch_size; ++n) {
        feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);

        update_builder << "$set"
                        << open_document
                        << caffeModelName + "_" + blob_names[i] << open_document
			<< "extraction_date" << bsoncxx::types::b_date(std::chrono::system_clock::now())
                        << "feature_height" << feature_blob->height()
                        << "feature_width " << feature_blob->width()
                        << "number_of_channels" << feature_blob->channels()
                        << "features_array_size" << dim_features
                        << "feature_array_data"
                            << open_array <<
                            [&](bsoncxx::builder::stream::array_context<> arr){
                                for (int d = 0; d < dim_features; d++)
                                arr << feature_blob_data[d];
                            } // Note that we don't invoke the lambda
                            << close_array
                        << close_document
                        << close_document;

    }  // for (int n = 0; n < batch_size; ++n)
    auto result_of_imageSet = dbMongo[collectionName].update_one(filter_builder.view(),update_builder.view(),upsert);
  }
  return ;
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void ExtractFeature::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = feature_extraction_net->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void ExtractFeature::Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_)
    cv::resize(sample, sample_resized, input_geometry_);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;

  // mean proto file is defined so we subract bt mean value
  if(!meanProtoFile.empty()){
    cv::subtract(sample_float, mean_, sample_normalized); // IF USING MEAN BInary file
  }
   else {
    sample_normalized = sample_float;
   }

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data) == feature_extraction_net->input_blobs()[0]->cpu_data()) << "Input channels are not wrapping the input layer of the network.";
}



int main(int argc, char** argv) {
  return feature_extraction_pipeline<float>(argc, argv);
//  return feature_extraction_pipeline<double>(argc, argv);
}

template<typename Dtype>
int feature_extraction_pipeline(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  const int num_required_args = 5;
  if (argc < num_required_args) {
    LOG(ERROR)<<
    "This program takes in a trained network and an input data layer, and then"
    " extract features of the input data produced by the net.\n"
    "Usage: my_Extract_Features  Caffe_Model_Name  pretrained_net_param"
    "  feature_extraction_proto_file  extract_feature_blob_name1[,name2,...]"
    "  (Optional) [CPU/GPU] [DEVICE_ID=0]\n"
    "Note: you can extract multiple features in one pass by specifying"
    " multiple feature blob names and dataset names separated by ','."
    " The names cannot contain white space characters and the number of blobs"
    " and datasets must be equal.";
    return 1;
  }
  int arg_pos = num_required_args;

  arg_pos = num_required_args;
  if (argc > arg_pos && strcmp(argv[arg_pos], "GPU") == 0) {
    LOG(ERROR)<< "Using GPU";
    int device_id = 0;
    if (argc > arg_pos + 1) {
      device_id = atoi(argv[arg_pos + 1]);
      CHECK_GE(device_id, 0);
    }
    LOG(ERROR) << "Using Device_id=" << device_id;
    Caffe::SetDevice(device_id);
    Caffe::set_mode(Caffe::GPU);
  } else {
    LOG(ERROR) << "Using CPU";
    Caffe::set_mode(Caffe::CPU);
  }

  arg_pos = 0;  // the name of the executable  // arg_pos = 0

  string model_name(argv[++arg_pos]); // arg_pos = 1 // Name Of Caffe Model

  std::string pretrained_binary_proto(argv[++arg_pos]); // arg_pos = 2 // Caffe Model

  // Expected prototxt contains at least one data layer such as
  //  the layer data_layer_name and one feature blob such as the
  //  fc7 top blob to extract features.
  /*
   layers {
     name: "data_layer_name"
     type: DATA
     data_param {
       source: "/path/to/your/images/to/extract/feature/images_leveldb"
       mean_file: "/path/to/your/image_mean.binaryproto"
       batch_size: 128
       crop_size: 227
       mirror: false
     }
     top: "data_blob_name"
     top: "label_blob_name"
   }
   layers {
     name: "drop7"
     type: DROPOUT
     dropout_param {
       dropout_ratio: 0.5
     }
     bottom: "fc7"
     top: "fc7"
   }
   */


  std::string feature_extraction_proto(argv[++arg_pos]); //arg_pos = 3 // Proto.txt

  std::string extract_feature_blob_names(argv[++arg_pos]); // arg_pos = 4  // Model_layer we are using e.g. fc7

  string databaseName = "ImageData";
  string collectionName =  "image_set";

  ExtractFeature ef = ExtractFeature(model_name , pretrained_binary_proto , feature_extraction_proto , extract_feature_blob_names , databaseName , true , "/media/amogh/Data/NeuralNet_Models/imagenet_mean.binaryproto");

  bool forceUpdate = false; // If set to true it will update the features even if already present


  auto myDb = conn[databaseName];

    vector<string> modelnames = ef.getMongoFeatureModelNames();

    bsoncxx::builder::stream::document search_builder_images;

    search_builder_images.clear();

    // Re-Update the features if forceUpdate is set to true.
    if(!forceUpdate){
    	for(string mdName : modelnames){
    		search_builder_images << mdName << open_document << "$exists" << false << close_document;
    	}
    }

	// Extract Features of only those images whole features aren't present
    for (bsoncxx::document::view doc : myDb[collectionName].find(search_builder_images.view()) ) {
                    bsoncxx::document::element myE = doc["_id"];
                    ef.getFeatures( myE.get_utf8().value.to_string() , collectionName);
    }

 LOG(ERROR)<< "Extracted All Features";

  return 0;
}

