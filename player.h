//
// Created by geoswift1 on 1/23/19.
//

#ifndef GETNUMBERPLATE_PLAYER_H
#define GETNUMBERPLATE_PLAYER_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <thread>

#include <mutex>
#include "detectVehicle.h"


using namespace std;
using namespace cv;


class Player : public QObject{
Q_OBJECT
public:
    explicit Player(QObject *parent= 0);
    ~Player();
    void startDetection(string videoPath);
    mutex mutex_;
signals:
    void on_stop_dv();
private:
    string croppedImageDir = "images";
    QString imagePath();
    DetectVehicle dv;



};

#endif //GETNUMBERPLATE_PLAYER_H
