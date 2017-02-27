#ifndef CALCATEGORY_H
#define CALCATEGORY_H

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <exception>

using namespace std; // For the STD::
using namespace boost::filesystem; // For the boost::filesystem::


class CalCategory
{

    protected:

        int id;
        string category;
        string path;
        vector<string> vecImages; // this will store the path of all images
        vector<string> vecTrainImages;
        vector<string> vecTestImages;

    public:
        CalCategory(int = -1, string = "" , string="");


        //Getters
        string getCategory(); // This will return The name of this Category
        string getStringPath(); // The Path of the folder which holds all the images in this category
        int getID(); // The ID of this Category

        vector<string>& getAllImages(); // This Will Give you the path of images in this category
        vector<string>& getTrainImages(); // This Will Give you the path of images in this category
        vector<string>& getTestImages(); // This Will Give you the path of images in this category

        //Functions
        void addAllImagesFromFolder();
        void createTrainTestImages(int train = 10 , int test = 10); /* This will create two lists one for training and other for testing
                                                                     where numbers represents the number of images in those categories */

        virtual ~CalCategory(); // Destructor



    private:

};

#endif // CALCATEGORY_H
