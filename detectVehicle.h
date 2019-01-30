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
#include <zconf.h>
#include <sys/param.h>
#include <QtCore/QDir>
#include <alpr.h>
#include <iterator>
#include <regex>
#include "worker.h"
#include "Car.cpp"

using namespace std;
using namespace cv;

class DetectVehicle : public QObject
{
Q_OBJECT

public:
    DetectVehicle(QObject *parent = 0);
    ~DetectVehicle();
    void startDetection(string videoPath);
    void startDetect();
    vector<Rect> setFrame();
    void Capture_Frame(const Mat& _new_frame, Mat& _new_next_frame);
    void Display_Res();
    void Vehicle_Recogniton();
    Mat current_frame_;
    mutex mutex_;
    vector<int>compr;
    Mat frame;
    Mat nextFrame;
    int counter = 0;
    void loop();

    vector<Car> cars;
    vector<Car> previousCars;
    QString detectedImagePath;
    alpr::Alpr *openalpr;
    vector<Point> detectedPts;
    vector<Point> areaOfInterest();
signals:
    void on_stop();

//private slots:

public slots:
    void getLicensePlate(QString path ,QString plate);
    void stop();

private:
    string croppedImageDir = "images";
    QString imagePath();
//    licenseRecognition recognitionLicense;
    bool mStop = false;
//    licenseRecognition * recognitionLicense = new licenseRecognition();
};

#endif //COUNTINGCARS_DETECTVEHICLE_H
