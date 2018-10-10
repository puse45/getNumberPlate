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
#include "process.h"

class DetectVehicle : public QObject
{
Q_OBJECT

public:
    DetectVehicle(QObject *parent = 0);
    ~DetectVehicle();
    void startDetection();
signals:
    void on_stop();

//private slots:


public slots:
    void getLicensePlate(QString plate);

private:
    MyJob myjob;
};

#endif //COUNTINGCARS_DETECTVEHICLE_H
