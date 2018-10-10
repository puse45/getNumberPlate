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
#include <vector>
#include <alpr.h>
#include <iterator>
#include <iostream>
using namespace std;


class MyJob : public QObject{
Q_OBJECT
public:
    explicit MyJob(QObject *parent= 0);
    ~MyJob();
    void start(QString name);


signals:
    void imagePath(QString name);

public slots:
    void stop();

private:
    bool mStop;
//    string imagelocation;
    void processimage(QString path);

};
#endif //THREAD_PROCESS_H

