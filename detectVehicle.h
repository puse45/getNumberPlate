//
// Created by pinje on 10/10/18.
//

#ifndef COUNTINGCARS_DETECTVEHICLE_H
#define COUNTINGCARS_DETECTVEHICLE_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include <iostream>
#include <cmath>
#include <alpr.h>
#include <iterator>
#include <regex>
#include <mutex>

using namespace std;
using namespace cv;

class DetectVehicle
{

public:
    DetectVehicle();
    ~DetectVehicle();

    void Capture_Frame(const Mat& _new_frame, Mat& _new_next_frame);
    void Vehicle_Recogniton();
    Mat current_frame_;
    mutex mutex_;
    vector<int>compr;
    Mat frame;
    Mat nextFrame;
    int counter = 0;

//    QString detectedImagePath;
    alpr::Alpr *openalpr;
    vector<Point> detectedPts;
    vector<Point> areaOfInterest();


//private slots:
//public slots:
//    void stop();

private:
    string croppedImageDir = "images";
    string cleanPlateRecognised;
    bool mStop = false;
    string removeNewLine(string plateRecognised);
    string validateLicensePlate(string plateRecognised);
//    QString imagePath();
    string* platechar = new string;
    alpr::AlprResults results;
};

#endif //COUNTINGCARS_DETECTVEHICLE_H
