#include "MongoCaltech256.h"

MongoCaltech256::MongoCaltech256(string databaseName , string imCollection  , string calCollection ){
    //ctor
    this->dbName = databaseName;
    this->imagesCollection = imCollection;
    this->caltechCollection = calCollection;

    this->conn = std::move(mongocxx::client(mongocxx::uri{}));
    // Connecting to the mongoDB database
    this->dbMongo = this->conn[databaseName];

}

void MongoCaltech256::setupCaltech(){

    bsoncxx::builder::stream::document filter_builder_caltech , search_builder_images , filter_builder_images;
    mongocxx::options::find cal_find , img_find; // This speeds up the queries

    filter_builder_caltech << "_id" << 1 << "cat_Name" << 1;

    cal_find.projection(filter_builder_caltech.view());

    filter_builder_images << "_id" << 1;
    img_find.projection(filter_builder_images.view());


    // Read from category collection
    auto cursor_cal = dbMongo[this->caltechCollection].find({} , cal_find);
    for (bsoncxx::document::view doc : cursor_cal) {

        int id = doc["_id"].get_int32().value;
        string catName = doc["cat_Name"].get_utf8().value.to_string();

        Category cat = Category(id , catName);


        search_builder_images.clear();
        search_builder_images << "cat_ID" << id;

        // Read all images for that category
        auto cursor_img = dbMongo[this->imagesCollection].find(search_builder_images.view() , img_find );



        for (bsoncxx::document::view doc_img : cursor_img) {
            string img_name = doc_img["_id"].get_utf8().value.to_string();
            cat.addImage(img_name);
        }
        id2Category.insert(std::make_pair(id, cat));
    } // Category Collection
}

void MongoCaltech256::setClutterCategory(int clut){
    this->clutterCategory = clut;
}

int MongoCaltech256::getClutterID(){
    return this->clutterCategory;
}

const unordered_map<int,Category>& MongoCaltech256::getCategoryMap(){
    return this->id2Category;
}


string MongoCaltech256::getDbName(){
    return this->dbName;
}
string MongoCaltech256::getCaltechCollectionName(){
    return this->caltechCollection;
}
string MongoCaltech256::getImageCollectionName(){
    return this->imagesCollection;
}


MongoCaltech256::MongoCaltech256(const MongoCaltech256& other){
    //copy ctor
    this->conn = std::move(mongocxx::client(other.conn.uri())); // = std::move(other.conn); --- No idea whats wrong here

    this->dbName = other.dbName;
    this->caltechCollection = other.caltechCollection;
    this->imagesCollection = other.imagesCollection;
        // Connecting to the mongoDB database
    this->dbMongo = other.dbMongo; //this->conn[databaseName];
    this->clutterCategory = other.clutterCategory;
    this->id2Category = other.id2Category;
}

MongoCaltech256::~MongoCaltech256(){
    //dtor
}
