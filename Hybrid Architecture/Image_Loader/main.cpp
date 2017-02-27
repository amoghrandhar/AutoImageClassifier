    #include "../include/Caltech256.h"
    #include <boost/filesystem.hpp>
    #include <boost/algorithm/string.hpp>
    #include <iostream>
    #include <fstream>

    //Mongo DB -> BSON
    #include <bsoncxx/builder/stream/document.hpp>
    #include <bsoncxx/types.hpp>
    #include <bsoncxx/json.hpp>

    //Mongo DB -> MongoDB
    #include <mongocxx/client.hpp>
    #include <mongocxx/instance.hpp>
    #include <mongocxx/uri.hpp>

    using namespace std; // For the STD::
    using namespace boost::filesystem; // For the boost::filesystem::

    //For MONGO-DB
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::open_document;
    using bsoncxx::builder::stream::close_document;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::finalize;


    // This Method will Create a data file on disk which contains the image path of Caltech Image and the category label of that image.
    void createNewCaltechFile_ImagePath_CategoryID(string fileName , Caltech256 &c256);
    void addToMongoDB(string databaseName , Caltech256 &c256);


    int main(int argc, char *argv[])
    {


        Caltech256 c = Caltech256(string(argv[1]));

        cout << " START \n";

        try{
            c.setupCaltech();
        } catch (const MyPathException &msg) {
            cerr << msg.doesNotExist() << endl;
       }
        // Add the data to Mongo_db
        addToMongoDB("ImageData1", c);
        // Create a data_file
        createNewCaltechFile_ImagePath_CategoryID("cdc.txt" , c);



        return 0;
    }


    void createNewCaltechFile_ImagePath_CategoryID(string fileName , Caltech256 &c256){

       std::ofstream myFile;
       myFile.open(fileName);

        for ( CalCategory ct : c256.getCaltech256Vector()){

            vector<string> myVecTemp = ct.getAllImages();

            for (unsigned int i = 0 ; i < myVecTemp.size() ; i++){
                string imgPath = boost::algorithm::replace_all_copy(myVecTemp.at(i)," " , "\\ ");
                myFile << imgPath << " " << ct.getID() << endl ;
            }


        }

        cout << endl << fileName << " Caltech Data File Created . \n";

        myFile.close();
    }

    void addToMongoDB(string databaseName , Caltech256 &c256){
        mongocxx::instance inst{};
        mongocxx::client conn{mongocxx::uri{}};

        auto db = conn[databaseName];
        auto upsert = mongocxx::options::update();
        upsert.upsert(true);

        //This Loop is to add data to Category_set :-> Key : Cat_ID , values :  category Name , catgory Path
        for ( CalCategory ct : c256.getCaltech256Vector()){
            vector<string> myVecTemp = ct.getAllImages();

            //This Loop is to add data to Image_set :-> Key : Full_Image_path , values : category ID , category Name
            for (unsigned int i = 0 ; i < myVecTemp.size() ; i++){
                string imgPath = boost::algorithm::replace_all_copy(myVecTemp.at(i)," " , "\\ ");
                bsoncxx::builder::stream::document filter_builder, update_builder;

                filter_builder <<"_id" << imgPath;
                update_builder << "$set"
                            << open_document
                            << "use_for_svm" << true
                            << "cat_ID" << ct.getID()
                            << close_document;

                auto result_of_imageSet = db["image_set"].update_one(filter_builder.view(),update_builder.view(),upsert);
            }

            bsoncxx::builder::stream::document filter_cat, update_cat;

                filter_cat <<"_id" << ct.getID();
                update_cat << "$set"
                            << open_document
                            << "cat_Name" << ct.getCategory()
                            << "cat_path" << ct.getStringPath()
                            << close_document;

                auto result_of_catSet = db["category_set"].update_one(filter_cat.view(),update_cat.view(),upsert);
        }
        cout << ">>> Finished Adding to MongoDB database \n";
    }
