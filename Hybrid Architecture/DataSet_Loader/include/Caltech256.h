#ifndef CALTECH256_H
#define CALTECH256_H

#include <boost/filesystem.hpp>
#include <iostream>
#include <exception>
#include "CalCategory.h"
#include <unordered_map>

using namespace std; // For the STD::
using namespace boost::filesystem; // For the boost::filesystem::

// This is the Exception Thrown if PATH PASSED IS NOTH A DIRECTORY
struct MyPathException : public exception
{

    string loc , message;
    MyPathException(string l , string m){
        loc = l;
        message = m;
    }

    const string doesNotExist () const throw ()
    {
        return ("\"" + loc + "\"" + " -> " + message);
    }
};

//The Class which holds the CALTECH256 data
class Caltech256
{

    protected:
        vector<CalCategory> categories;

        unordered_map<int,CalCategory> id2Category;



    public:
        Caltech256(string path_to_Caltech256); // Constructor -> path_to_caltech_256 folder needed

        void setupCaltech(); // To setup the caltech Data -> Run only after intialising.

        vector<CalCategory>& getCaltech256Vector(); // This Will Provide with a Vector Of Categories

        unordered_map<int,CalCategory>& getCaltech256IDMap(); // This will provide a Map of Id to Category Mapping

        CalCategory& getImageCategory_FROM_ID(int id = -1); // This Will give The Category from The category_ID

        virtual ~Caltech256(); // Destructor


    private:
    path pc256;
    void processFolder(path folder); // This will process each directory


};

#endif // CALTECH256_H
