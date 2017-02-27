#include "ImageGetFeature.h"

ImageGetFeature::ImageGetFeature(string databaseName , string imCollection  , string feature_model_layer_name)
{
    //ctor
    this->dbName = databaseName;
    this->imagesCollection = imCollection;
    this->feature_lay_model_name = feature_model_layer_name;
    // Connecting to the mongoDB database
    this->conn = std::move(mongocxx::client(mongocxx::uri{}));
    this->dbMongo = conn[databaseName];
}

vector<float> ImageGetFeature::getFeaturesImage(string imageName , string caffe_feature_model_layer_name , string collectionName){

    vector<float> img_features ;

    bsoncxx::builder::stream::document search_builder_images , filter_builder_images;
    mongocxx::options::find img_find; // This speeds up the queries

    search_builder_images.clear();
    search_builder_images <<  "_id" << imageName << caffe_feature_model_layer_name << open_document <<"$exists" << true << close_document ;

    filter_builder_images.clear();
    filter_builder_images << caffe_feature_model_layer_name << 1 ;
    img_find.projection(filter_builder_images.view());

    // Read from category collection
    boost::optional<bsoncxx::document::value> doc_value = dbMongo[collectionName].find_one(search_builder_images.view() , img_find);


    if(doc_value){
        bsoncxx::document::view doc_view = doc_value.get().view();
        bsoncxx::document::element be = doc_view[caffe_feature_model_layer_name];

        img_features.reserve(be.get_document().view()["features_array_size"].get_int32());

        //Getting the feature value array from mongodb and creating a c++ vector which contains the features for the image
        for (bsoncxx::array::element ele_array : be.get_document().view()["feature_array_data"].get_array().value) {
            img_features.push_back(ele_array.get_double()) ;
        }

    } else {
        throw std::out_of_range( caffe_feature_model_layer_name+ " feature data not found for Image : " + imageName + " \n please  extract the features for the image first ! " );
    }
    return img_features;
}

vector<float> ImageGetFeature::getFeaturesImage(string imageName , string collectionName){
    return this->getFeaturesImage(imageName, this->feature_lay_model_name , collectionName);
}

ImageGetFeature::ImageGetFeature(const ImageGetFeature& other){
    //copy ctor
    this->conn = std::move(mongocxx::client(other.conn.uri())); // = std::move(other.conn); --- No idea whats wrong here

    this->dbName = other.dbName;
    this->imagesCollection = other.imagesCollection;
    this->feature_lay_model_name = other.feature_lay_model_name;
        // Connecting to the mongoDB database
    this->dbMongo = other.dbMongo;


}

ImageGetFeature::~ImageGetFeature()
{
    //dtor
}
