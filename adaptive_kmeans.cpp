//
//  adaptive_kmeans.cpp
//  Project
//
//  Created by Tommaso Ruscica on 27/08/15.
//  Copyright (c) 2015 Tommaso Ruscica. All rights reserved.
//

#include "adaptive_kmeans.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <numeric>


using namespace std;
using namespace cv;




void Adaptive_Kmeans(vector <Mat> images){
    //check image's color
    //Here i check only first image.TODO insert loop for each images in vector "images"
    if(images[0].channels()==3){
        //Color image
        ColorClustering(images);
    }else{
        //Gray Image
        //TODO write code here for gray image
    }
}


void ColorClustering(vector <Mat> images){
    //TODO loop single image in vector "images"
    vector<Vec3f> center;
    
    //get 3 channels from image and store in a Mat
    vector<Mat> array_color;
    split(images[0],array_color);
    
    Mat blue = array_color[0];
    Mat green = array_color[1];
    Mat red = array_color[2];
    
    //"array" is a image matrix with a channel in a single column, it have 3 cols(red,green,blue)
    Mat array=Mat(images[0].cols*images[0].rows, 3, CV_8UC1, images[0].data);
    
    //size image, rows and columns
    int image_rows = images[0].rows;
    int image_cols = images[0].cols;
    cv::Size s = images[0].size();
    image_rows = s.height;
    image_cols = s.width;
    
    //size rows of "array"
    int dim_row=array.rows;
    
    //first temp vectors used in loop while for searching centers
    vector<double> vettore_red;
    vector<double> vettore_green;
    vector<double> vettore_blue;
    
    for(int x=0;x<array.rows;x++){  //inizializing temp vectors with relative cols of "array"
        vettore_red.push_back(array.at<uchar>(x,2));    //RED channel
        vettore_green.push_back(array.at<uchar>(x,1));  //GREEN channel
        vettore_blue.push_back(array.at<uchar>(x,0));   //BLUE channel
    }
    
    //binary vector used to delete from iterations values of first temp vectors
    vector<double> qualified;
    
    //i=number's clusters
    //j=number's iterations
    int i = 0 , j = 0;
    
    while(true){    //first WHILE
        
        //"seed"= vector with mean's value of each channel
        Scalar r=mean(vettore_red);
        Scalar b=mean(vettore_blue);
        Scalar g=mean(vettore_green);
        
        Vec3f seed = {static_cast<float>(r.val[0]),static_cast<float>(g.val[0]),static_cast<float>(b.val[0])};
        //cout<<"SEEDS:"<<endl;
        //cout<<seed[0]<<endl;
        //cout<<seed[1]<<endl;
        //cout<<seed[2]<<endl;
        //cout<<"------"<<endl;
        
        //Number of clusters
        i = i + 1;
        
        while(true){    //second WHILE
            //number of iterations
            j = j + 1;
            
            //vector containing euclidean distance
            vector<double> dist;
            
            //cout<<"SIZE:ARRAY"<<endl;
            //cout<< vettore_red.size()<<endl;
            //cout<<"--------"<<endl;
            for(int x=0;x<vettore_red.size();x++){
                //NOTE: euclidean distance from example:
                //http://www.mathworks.com/matlabcentral/fileexchange/45057-adaptive-kmeans-clustering-for-color-and-gray-image/content/adaptcluster_kmeans.m
                double dist1 = sqrt(pow(vettore_red[x]-seed[0], 2.0));
                double dist2 = sqrt(pow(vettore_green[x]-seed[1], 2.0));
                double dist3 = sqrt(pow(vettore_blue[x]-seed[2], 2.0));
                dist.push_back(dist1+dist2+dist3);
            }
            
            //get maximum value from vector dist
            double distth = *max_element(dist.begin(), dist.end())*0.25;
            //cout<<distth<<endl;
            
            //second temp vectors
            vector<double> array_red;
            vector<double> array_green;
            vector<double> array_blue;
            
            //set 1 in qualified vector if value of dist is < of maximum value "distth", else 0
            //delete from first temp vector the values that corresponding 0 in qualified
            for (int x=0;x<dist.size();x++){
                if (dist[x] < distth){
                    qualified.push_back(1);
                    array_red.push_back(vettore_red[x]);
                    array_green.push_back(vettore_green[x]);
                    array_blue.push_back(vettore_blue[x]);
                }
                else{
                    qualified.push_back(0);
                }
            }
            dist.clear();
            
            //Recalculates means from new vectors
            Scalar new_red=mean(array_red);
            Scalar new_green=mean(array_green);
            Scalar new_blue=mean(array_blue);
            
            Vec3f new_seed = {static_cast<float>(new_red.val[0]),static_cast<float>(new_green.val[0]),static_cast<float>(new_blue.val[0])};
            //cout<<"NEW_SEEDS:"<<endl;
            //cout<< new_seed[0] <<endl;
            //cout<<"------"<<endl;
            
            //exit from second while loop if new_seed is NaN(not a number)
            if(new_seed[0]==0 | new_seed[1]==0 | new_seed[2]==0){
                //cout<< "NaN" <<endl;
                break;
            }
            
            //exit from second while loop if convergence reached or after 10 iterations
            if((seed == new_seed) | j>10){
                //cout<< "SEED=NEWSEED" <<endl;
                j=0;
                
                //cout<<"SIZE:ARRAY"<<endl;
                //cout<< vettore_red.size()<<endl;
                //cout<<"SIZE:QUALIFIED"<<endl;
                //cout<< qualified.size()<<endl;
                //cout<<"--------"<<endl;
                
                //third temp vectors
                vector<double>vettore_red3;
                vector<double>vettore_green3;
                vector<double>vettore_blue3;
                
                //get from each channel(cols) of "array" the values that corresponding 0 in vector "qualified" and repeat second while loop
                //Remove values which have assigned to a cluster
                for(int x=0 ; x < qualified.size() ; x++){
                    if(qualified[x]==0){
                        vettore_red3.push_back(vettore_red[x]);
                        vettore_green3.push_back(vettore_green[x]);
                        vettore_blue3.push_back(vettore_blue[x]);
                    }
                }
                vettore_red.clear();
                vettore_green.clear();
                vettore_blue.clear();
                vettore_red=vettore_red3;
                vettore_green=vettore_green3;
                vettore_blue=vettore_blue3;
                //cout<<"SIZE_ARRAY: "<<vettore_red.size()<<endl;
                
                //Certer vector
                center.push_back({ new_seed[0],new_seed[1],new_seed[2] });
                //cout<< center.size() <<endl;
                break;
            }
            
            //update seeds
            seed[0] = new_seed[0];
            seed[1] = new_seed[1];
            seed[2] = new_seed[2];
            
            qualified.clear();
            
        }//end second WHILE
        
        if(array.empty() || i>10){
            //cout<< "ARRAY_EMPTY" <<endl;
            i=0;
            break;
        }
        
    }  //end first WHILE
    
    
    vector<double> centers;
    for(int x=0 ; x < center.size() ; x++){
        double p=pow(center[x][0],2.0);
        double p1=pow(center[x][1],2.0);
        double p2=pow(center[x][2],2.0);
        double p_tot=p+p1+p2;
        double c = sqrt(p_tot);
        centers.push_back(c);
        
    }
    //Sort Centers
    vector<int> idx;
    vector<double> centers_idx=centers;
    sort(centers.begin(), centers.end());
    
    int k=1;
    for(int i=0;i<centers.size();i++){
        double temp=centers[i];
        //cout<<temp<<endl;
        for(int x=0;x<centers_idx.size();x++){
            if(temp==centers_idx[x]){
                idx.push_back(k+x);
            }
        }
    }
    
    //cout<< "IDX"<<endl;
    //for(int i=0; i<idx.size(); i++){
    //cout << idx[i] << " ";
    //}
    //cout<<endl;
    
    
    int intercluster=25;
    vector<int> a;
    vector<double> centers_temp;
    int total;
    
    while(true){    //third WHILE
        vector<float> new_centers(centers.size());
        //Find out Difference between two consecutive Centers
        //adjacent_difference(first,last,results)
        adjacent_difference(centers.begin(), centers.end(), new_centers.begin());
        new_centers.erase(new_centers.begin()+0);
        
        //for(i=0; i<centers.size(); i++){
        //cout << new_centers[i] << " ";
        //cout<< endl;
        //}
        
        //Findout Minimum distance between two cluster Centers
        //Discard Cluster centers less than distance
        int k=0;
        for(int i=0;i<centers.size();i++){
            if(new_centers[i]<intercluster){
                a.push_back(1);
                if(k==0){
                    //center.erase(center.begin()+i);
                    idx.erase(idx.begin()+i);
                }else{
                    //center.erase(center.begin()+i-k);
                    idx.erase(idx.begin()+i-k);
                }
                k++;
            }else{
                a.push_back(0);
            }
        }
        
        for(int i=0 ; i < centers.size() ; i++){
            if(a[i]==0 ){
                centers_temp.push_back(centers[i]);
            }
        }
        centers.clear();
        centers=centers_temp;
        centers_temp.clear();
        
        //nnz function
        total=0;
        for(int i=0;i<a.size();i++){
            total+=a[i];
        }
        a.clear();
        
        if(total==0){
            //for(i=0; i<new_centers.size(); i++)
            //cout << new_centers[i] << " ";
            //cout<< endl;
            break;
        }
    } //end third WHILE
    
    
    vector<Vec3f> center_temp;
    for(int i=0;i<idx.size();i++){
        int k=idx[i];
        center_temp.push_back({ center[k-1][0], center[k-1][1], center[k-1][2] });
    }
    center=center_temp;
    
    cout<< "CENTER"<<endl;
    for(i=0; i<center.size(); i++){
        cout << center[i] << " ";
    }
    cout<<endl;
    
    cout<< "CENTERS"<<endl;
    for(i=0; i<center.size(); i++){
        cout << centers[i] << " ";
    }
    cout<<endl;
    
    vector<float> distred;
    vector<float> distgreen;
    vector<float> distblue;
    
    float distance[dim_row][center.size()];
    
    //Find distance between center and pixel value.
    for(int i=0;i<center.size();i++){
        for(int x=0;x<dim_row;x++){
            distred.push_back(pow(array.at<uchar>(x,2)-center[i][0],2));
            distgreen.push_back(pow(array.at<uchar>(x,1)-center[i][1],2));
            distblue.push_back(pow(array.at<uchar>(x,0)-center[i][2],2));
            
            distance[x][i]=sqrt(distred[0]+distgreen[0]+distblue[0]);
            distred.clear();
            distgreen.clear();
            distblue.clear();
        }
    }
    
    vector<int> label;
    
    //Choose cluster index of minimum distance
    //Calculate min value each row of vector "distance" and write his column's number in "label"
    for(int x=0;x<dim_row;x++){
        int j=0;
        int k=0;
        for(int i=0;i<center.size();i++){
            float h;
            if(i==0){//first value(first col)
                j=1;
                h=distance[x][i];
            }else if(distance[x][i]<h){//other cols
                j=j+k;
                h=distance[x][i];
                k=0;
            }
            k++;
        }
        label.push_back(j);
    }
    
    //for(int i=0;i<dim_row;i++){
    //if(label[i]==5){
    //x=x+1;
    //}
    //}
    //cout << x << endl;
    
    //reshape image with each pixel labelled
    Mat label_final = Mat(label).reshape(0,image_rows);
    
    //cout<<label_final.rows<<endl;
    //cout<<label_final.cols<<endl;
    
    //Convert Mat int to float
    Mat_<float> image_float;
    label_final.convertTo(image_float,CV_32F);
    
    //cout<<image_clustered.rows<<endl;
    //cout<<image_clustered.cols<<endl;
    
    //--------------------------------------
    //Looking for max and min value in Mat
    //Initialize m
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    minMaxLoc( image_float, &minVal, &maxVal, &minLoc, &maxLoc );
    cout << "min val : " << minVal << endl;
    cout << "max val: " << maxVal << endl;
    //--------------------------------------
    
    //create and display Mat with clusters in grayscale
    Mat img=image_float/maxVal;
    //cout<<img.rows<<endl;
    //cout<<img.cols<<endl;
    
    imshow( "Display window", img );
    waitKey(0);
    
}