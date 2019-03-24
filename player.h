//
// Created by geoswift1 on 1/23/19.
//

#ifndef GETNUMBERPLATE_PLAYER_H
#define GETNUMBERPLATE_PLAYER_H

#include <iostream>
#include <thread>

#include <mutex>
#include "detectVehicle.h"


using namespace std;
using namespace cv;


class Player{
public:
    explicit Player();
    ~Player();
    void startDetection(string videoPath);
    mutex mutex_;
//signals:
//    void on_stop_dv();
private:
    DetectVehicle dv;
    int npts;
};

#endif //GETNUMBERPLATE_PLAYER_H
