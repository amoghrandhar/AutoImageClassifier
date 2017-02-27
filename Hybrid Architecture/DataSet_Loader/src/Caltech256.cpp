    #include "../include/Caltech256.h"
    #include "../include/CalCategory.h"

    #include <boost/filesystem.hpp>
    #include <iostream>


    using namespace std; // For the STD::
    using namespace boost::filesystem; // For the boost::filesystem::

    //ctor
    Caltech256::Caltech256(string path_to_Caltech256)
    {

        cout << " \n \a \t \t =======> CALTECH 256 <======= \n\n " << endl;
        pc256 = path(path_to_Caltech256);
        categories = vector<CalCategory>();

    }


    void Caltech256::setupCaltech(){
        //Directory doesn't Exist
        if(!exists(pc256) ){
            throw MyPathException(pc256.string(),string("Not A Directory"));
        }
        //Directory Exist
        else if(is_directory(pc256)){
            // Loop Over All folders
            for (directory_entry &x : directory_iterator(pc256)){
                    processFolder(x.path());
            }
        }
        return;
    }

    void Caltech256::processFolder( path folder){

        if(is_directory(folder)){
            string folderName = folder.filename().string();
            string category = folderName.substr(1 + folderName.find(".") ,folderName.length());
            int id = atoi(folderName.substr(0 ,folderName.find(".")).c_str());
            string path = absolute(folder).string();

            //Creating a category of that folder
            CalCategory cat = CalCategory(id,category,path);
            cat.addAllImagesFromFolder();

            //Add to Our ID_CATEGORY MAP
            this->id2Category[id] = cat;
            this->categories.push_back(cat);
        } else {
            cout << " \n >>>>> " <<folder.string() << " Is not a Correct Folder . \n";
        }
    }


    vector<CalCategory>& Caltech256::getCaltech256Vector(){
        return this->categories;
    }

    unordered_map<int, CalCategory>& Caltech256::getCaltech256IDMap(){
        return this->id2Category;
    }

    CalCategory& Caltech256::getImageCategory_FROM_ID(int id){
        return this->id2Category[id];
    }

    Caltech256::~Caltech256()
    {
        //dtor
    }
