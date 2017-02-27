This is my repo for Indiviual Project

>> Dependencies Needed to build
	> Caffe
		>> Build on the basis of Make Files present in Caffe_changes_done folder
	> Mongodb 3.2 or up
		>> Server up and running
		>> C++ bindings >> 3.2 or newwer one
	> Open CV



Check The pdf report which explains the working.

File Structure shown in Tree.txt file


# One Mongo Bug found : >> File Affected : ImageGetFeature.cpp 
#			>> Code poit : search_builder_images <<  "_id" << imageName ; //<< caffe_feature_model_layer_name << open_document <<"$exists" << true << close_document ;


#In ImageGetFeature ::

        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        //Creating a vector containing features data
        for(int i=0 ; i < be.get_document().view()["features_array_size"].get_int32() ; i++ ) {
            img_features.push_back(be.get_document().view()["feature_array_data"].get_array().value[i].get_double()) ;
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>( t2 - t1 ).count();
        cout << "\n\n\nTime Taken Method 1 :: " << duration << endl << " Vec Size : " << img_features.size() << endl;

        t1 = high_resolution_clock::now();
        for (bsoncxx::array::element ele_array : be.get_document().view()["feature_array_data"].get_array().value) {
            img_features1.push_back(ele_array.get_double()) ;
        }
        t2 = high_resolution_clock::now();
        duration = duration_cast<microseconds>( t2 - t1 ).count();
        cout << "\n\n\nTime Taken Method 2 :: " << duration << endl << " Vec Size : " << img_features1.size() << endl;;

        bool are_equal = img_features == img_features1;
        cout << " Are 2 vectors equal : " << are_equal << endl ;

 #Time Taken Method 1 :: 144172
 #Vec Size : 4096

 #Time Taken Method 2 :: 393
 #Vec Size : 4096
 
 #Are 2 vectors equal : 1

On doing the above test and benchmark , I found the method is way faster than method 1 , and also that both method return same vectors so choosing method 2

