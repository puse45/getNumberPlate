#include <iostream>
#include "detectVehicle.h"
#include "player.h"

int main(int argc, char *argv[])
{

//    DetectVehicle dvehicle;
//    dvehicle.startDetection("/home/geoswift1/Videos/testVids/plate5.mp4");
    Player player;
    player.startDetection("/home/pinje/Videos/testVids/plate5.mp4");
    return 0;
}

