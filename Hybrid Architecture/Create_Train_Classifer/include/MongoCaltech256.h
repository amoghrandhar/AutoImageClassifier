#ifndef MONGOCALTECH256_H
#define MONGOCALTECH256_H

//Standard Inputs
#include <unordered_map>
#include <vector>
#include <iostream>

//Boost Includes
#include <boost/utility/string_ref.hpp> // To convert string_ref to string

//Mongo DB -> BSON
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>


//Mongo DB -> MongoDB
#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/find.hpp>

// Catergory
#include <Category.h>


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;



//For My Ease
using namespace std;


class MongoCaltech256
{
    public:



        /* Constructor */
        MongoCaltech256(string databaseName = "ImageData" , string imCollection = "image_set" , string calCollection = "category_set");
        /* Copy Constructor */
        MongoCaltech256(const MongoCaltech256& other);
        /* To setup the Caltech Class */
        void setupCaltech();
        /* To update the category id to be used as clutter */
        void setClutterCategory(int clut);
        /* To get the clutter category ID */
        int getClutterID();

        /* To get the name of DB */
        string getDbName();
        /* To get the name of Caltech Collection Table */
        string getCaltechCollectionName();
        /* To get the name of Image data collection Table */
        string getImageCollectionName();

        /* To get the unordered map */
        const unordered_map<int,Category>& getCategoryMap();



        //Destructor
        virtual ~MongoCaltech256();


    protected:

        // MongoDB Client Parameter
        mongocxx::client conn;
        mongocxx::database dbMongo;

        string dbName;
        string caltechCollection;
        string imagesCollection;

        unordered_map<int,Category> id2Category;
        int clutterCategory = 257; //The default one

    private:
};

#endif // MONGOCALTECH256_H
