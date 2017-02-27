    #include "../include/Category.h"

    #include <boost/algorithm/string.hpp>
    #include <boost/filesystem.hpp>
    #include <iostream>
    #include <exception>
    using namespace std; // For the STD::
    using namespace boost::filesystem; // For the boost::filesystem::

    Category::Category(int id , string cat){
        //ctor
        this->id = id;
        this->category = cat;
        this->vecImages.reserve(100);
    }


    void Category::createTrainTestImages(int tr , int ts){
        if(tr == -1){
            tr = vecImages.size();
            ts = 0;
        }
        else if(ts > 0 && (unsigned)ts < vecImages.size() && tr < -1){
            tr = vecImages.size() - ts;
        }
        else if (((unsigned)tr + ts) > vecImages.size()){
        cout << "ERROR : \n" << endl;
            throw std::out_of_range("Total Number of images in >> " +
                        boost::algorithm::to_upper_copy<std::string>(this->category)  +
                        " << is less than Train " + std::to_string(tr) + " Test " +
                        std::to_string(ts) + ", Total : " +
                        to_string(vecImages.size()) + "\n" );
        }

        std::srand(std::time(0));
        std::random_shuffle(vecImages.begin(),vecImages.end() );


        for(int i = 0 ; i < tr ; i++){
            vecTrainImages.push_back(vecImages.at(i));
        }
        for(int i = tr  ; i < (ts + tr) ; i++){
            vecTestImages.push_back(vecImages.at(i));
        }

    }

    void Category::createTrainTestImages(float tr, float ts){

        if ((tr + ts) > 1 || tr < 0 || ts < 0 ){
        cout << "ERROR : \n" << endl;
            throw std::out_of_range("Error on argument values float %train or float %test " + boost::algorithm::to_upper_copy<std::string>(this->category)  +" << is more than 1 (100%) or either of them is -ve  \n");
        }

        int totalImages = vecImages.size();
        this->createTrainTestImages( static_cast<int> (totalImages * tr) , static_cast<int> (totalImages * ts + 0.5));
    }


    void Category::addImage(string imageName){
        this->vecImages.push_back(imageName);
    }


    void Category::setAllImages(vector<string>& images){
        this->vecImages = images;
    }


    vector<string>& Category::getAllImages(){
        return this->vecImages;
    }


    vector<string>& Category::getTrainImages(){
        return this->vecTrainImages;
    }

    vector<string>& Category::getTestImages(){
        return this->vecTestImages;
    }


    string Category::getCategory(){
        return this->category;
    }


    int Category::getID(){
        return this->id;
    }


    Category::Category(const Category& other)
    {
        //copy ctor
        this->id = other.id;
        this->category = other.category;
        this->vecImages = other.vecImages;
        this->vecTestImages = other.vecTestImages;
        this->vecTrainImages = other.vecTrainImages;
    }

    Category::~Category()
    {
        //dtor
    }
