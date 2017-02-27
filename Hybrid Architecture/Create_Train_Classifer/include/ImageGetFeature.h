#ifndef IMAGEGETFEATURE_H
#define IMAGEGETFEATURE_H

//Standard Inputs
#include <vector>
#include <iostream>
#include <chrono>
#include <exception>

//Boost Includes
#include <boost/utility/string_ref.hpp> // To convert string_ref to string
#include <boost/optional.hpp> // Optional , used by find one

//Mongo DB -> BSON
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>


//Mongo DB -> MongoDB
#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/find.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

using namespace std;

class ImageGetFeature
{
    public:
        //Variables
        string dbName;
        string imagesCollection;
        string feature_lay_model_name;

    public:
        ImageGetFeature( string databaseName = "ImageData" , string imCollection = "image_set" , string feature_model_layer_name="");

        /* Copy Constructor */
        ImageGetFeature(const ImageGetFeature& other);

        /* This will return the features of that image , params : imageName , caffe_modelName_layerName , collectionName(Optional) */
        vector<float> getFeaturesImage(string image , string caffe_feature_model_name, string collectionName = "image_set");
        /* This will return the features of that image , params : imageName , collectionName(Optional) */
        vector<float> getFeaturesImage(string image , string collectionName = "image_set");


        /* Destructor */
        virtual ~ImageGetFeature();

    protected:
        mongocxx::client conn;
        mongocxx::database dbMongo;


    private:
};

#endif // IMAGEGETFEATURE_H
