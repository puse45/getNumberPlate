#include <iostream>
#include <QtWidgets/QApplication>
#include "detectVehicle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DetectVehicle dvehicle;
    dvehicle.startDetection();
    return a.exec();
}

