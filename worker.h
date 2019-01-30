//
// Created by geoswift1 on 10/9/18.
//

#ifndef THREAD_PROCESS_H
#define THREAD_PROCESS_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<dirent.h>
#include <thread>
#include <vector>
#include <alpr.h>
#include <iterator>
#include <iostream>
#include <regex>
#include <algorithm>
#include <string>
#include <mutex>

using namespace std;


class licenseRecognition : public QObject{
Q_OBJECT
public:
    explicit licenseRecognition(QObject *parent= 0);
    ~licenseRecognition();
    void start(QString path);
    void test(QString path);
    void executes();
    mutex mutex_;


signals:
    void imagePath(QString path,QString plate);

public slots:
    void stop();

private:
    bool mStop;
    string cleanPlateRecognised;
//    string imagelocation;
//    void processimage(QString path);
    string validateLicensePlate(string plateRecognised);
    string removeNewLine(string plateRecognised);

};
#endif //THREAD_PROCESS_H

