    #include "../include/CalCategory.h"

    #include <boost/algorithm/string.hpp>
    #include <boost/filesystem.hpp>
    #include <iostream>
    #include <exception>
    using namespace std; // For the STD::
    using namespace boost::filesystem; // For the boost::filesystem::

    CalCategory::CalCategory(int id , string cat , string path){
        //ctor
        this->id = id;
        this->category = cat;
        this->category[0] = toupper(this->category[0]);
        this->path = path;
        this->vecImages.reserve(100);
    }

    void CalCategory::addAllImagesFromFolder(){
        boost::filesystem::path folder = boost::filesystem::path(this->path);
        for(directory_entry &x : directory_iterator(folder)){
            if( is_regular_file(x.path()) && x.path().extension() == ".jpg"){
                vecImages.push_back(x.path().string());
            }
        }

    }

    void CalCategory::createTrainTestImages(int tr , int ts){

        cout << " VECTOR SIZE : " << vecImages.size() << "   TR + TS " << (unsigned)tr + ts  << endl ;

        if (((unsigned)tr + ts) > vecImages.size()){
        cout << "error" << endl;
            throw std::out_of_range("Total Number of images in >> " + boost::algorithm::to_upper_copy<std::string>(this->category)  +" << is less than Train + Test , Total : " + to_string(vecImages.size()));
        }else {
            std::random_shuffle(vecImages.begin(),vecImages.end() );

            for(int i = 0 ; i < tr ; i++){
                vecTrainImages.push_back(vecImages.at(i));
            }
            for(int i = tr  ; i < (ts + tr) ; i++){
                vecTestImages.push_back(vecImages.at(i));
            }
        }
    }

    vector<string>& CalCategory::getAllImages(){
        return this->vecImages;
    }

    vector<string>& CalCategory::getTrainImages(){
        return this->vecTrainImages;
    }

    vector<string>& CalCategory::getTestImages(){
        return this->vecTestImages;
    }


    string CalCategory::getCategory(){
        return this->category;
    }

    string CalCategory::getStringPath(){
        return this->path;
    }

    int CalCategory::getID(){
        return this->id;
    }


    CalCategory::~CalCategory()
    {
        //dtor
    }
