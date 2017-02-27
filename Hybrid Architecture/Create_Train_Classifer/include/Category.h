#ifndef CATEGORY_H
#define CATEGORY_H

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <cmath>
#include <exception>

using namespace std; // For the STD::
using namespace boost::filesystem; // For the boost::filesystem::


class Category
{

    protected:

        int id;
        string category;
        vector<string> vecImages; // this will store the path of all images
        vector<string> vecTrainImages;
        vector<string> vecTestImages;

    public:
        Category(int catID = -1, string catName = "");


        //Getters
        string getCategory(); // This will return The name of this Category
        int getID(); // The ID of this Category

        void addImage(string imageName);
        void setAllImages(vector<string>& images);


        vector<string>& getAllImages(); // This Will Give you the path of images in this category
        vector<string>& getTrainImages(); // This Will Give you the path of images in this category
        vector<string>& getTestImages(); // This Will Give you the path of images in this category

        //Functions
        void addAllImagesFromFolder();
        void createTrainTestImages(int train = -1 , int test = 0); /* This will create two lists one for training and other for testing
                                                                     where numbers represents the number of images in those categories
                                                                     BY default, "-1 means "it will create a list of all images in training and 0 in testing
                                                                     Other Case -> if tr = -2 and ts > 0 and ts < totalImages then tr becomes the left over images
                                                                     else -> it tr > 0 and ts > 0 then then number of training and testing images = tr , ts
                                                                     */
        void createTrainTestImages(float train = 0.9f , float test = 0.1f); /* This will create two lists one for training and other for testing
                                                                     where numbers represents the percentage of images in those categories
                                                                     BY default, "0.9f means "it will create a list of 90% images for training and (0.1f = 10%) in testing

                                                                     -> train + test shall be less than or equal to 1 and both the values shall be between [0-1].
                                                                     */


        Category(const Category& other); // Copy Constructor
        virtual ~Category(); // Destructor



    private:

};

#endif // CALCATEGORY_H
