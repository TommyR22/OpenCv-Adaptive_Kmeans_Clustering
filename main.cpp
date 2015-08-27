//
//  main.cpp
//  Project
//
//  Created by Tommaso Ruscica on 20/07/15.
//  Copyright (c) 2015 Tommaso Ruscica. All rights reserved.
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <dirent.h>
#include "adaptive_kmeans.h"


using namespace std;
using namespace cv;


int main(int argc, const char * argv[]) {
    
    //path image's directory
    string directoryName = "/Users/TommyR22/Desktop/SegTrackv2/JPEGImages/CSI/";
    //string directoryName = "/Users/TommyR22/Desktop/videos/penguin/images/";
    
    //path's vector of images in directoryName
    std::vector <string> path_images;
    //Mat's vector of images in directoryName
    std::vector <Mat> images;
    
    DIR *dir;
    dir = opendir(directoryName.c_str());
    string imgName;
    struct dirent *ent;
    
    //loop for read all images in the directoryName
    if (dir != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            imgName= ent->d_name;
            //read file images except OS's files like "." , ".." , "DS_Store"
            if(imgName.compare(".")!= 0 && imgName.compare("..")!= 0 && imgName.compare(".DS_Store")!= 0)
            {
                string path;
                path.append(directoryName);
                path.append(imgName);
                Mat image= imread(path);
                images.push_back(image);
                path_images.push_back(path);
                //cout << path << endl;
            }
        }
        closedir (dir);
    }  else {
        cout<<"Directory not present"<<endl;
    }
    cout << "images in vector: "+ to_string(images.size()) << endl;
    
    //Algorithm Adaptive K-Means
    Adaptive_Kmeans(images);
    
    
    return 0;
}