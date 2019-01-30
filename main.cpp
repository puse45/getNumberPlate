#include <iostream>
#include <QtWidgets/QApplication>
#include "detectVehicle.h"
#include "player.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    DetectVehicle dvehicle;
//    dvehicle.startDetection("/home/geoswift1/Videos/testVids/plate5.mp4");
    Player player;
    player.startDetection("/home/geoswift1/Videos/testVids/plate5.mp4");
    return a.exec();
}

