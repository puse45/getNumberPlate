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
#include "worker.h"

class DetectVehicle : public QObject
{
Q_OBJECT

public:
    DetectVehicle(QObject *parent = 0);
    ~DetectVehicle();
    void startDetection(string videoPath);
signals:
    void on_stop();

//private slots:

public slots:
    void getLicensePlate(QString path ,QString plate);

private:
    string croppedImageDir = "images";
    QString imagePath();
    licenseRecognition recognitionLicense;
};

#endif //COUNTINGCARS_DETECTVEHICLE_H
