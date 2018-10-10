//
// Created by geoswift1 on 10/9/18.
//

#include "process.h"
#include <iostream>

using namespace std;

MyJob::MyJob(QObject *parent) : QObject(parent)
{
    mStop = false;
}
MyJob::~MyJob() {

}

void MyJob::start(QString name) {
    qDebug() << "Signal name " << name;
    mStop = false;
        if(mStop)return;
        processimage(name);
        QThread::currentThread()->msleep(100);
//        emit imagePath(name);
}

void MyJob::stop() {
    mStop = true;

}

void MyJob::processimage(QString path) {
    string imagelocation = path.toStdString();
    string* bestPlate = new string;
    std::vector< string >  platerecimagepath;
//    std::cout << "Image Path "<< imagelocation << endl;
    alpr::Alpr *openalpr= new alpr::Alpr("eu,vn2", "/etc/openalpr/openalpr.conf");
    openalpr->setTopN(20);
    // comparing the plate text with the regional pattern.
    openalpr->setDefaultRegion("md");
    // Make sure the library loaded before continuing.
    // For example, it could fail if the config/runtime_data is not found
    if (!openalpr->isLoaded())
    {
        std::cerr << "Error loading OpenALPR" << std::endl;
//        return 1;
    }

    // Recognize an image file.  You could alternatively provide the image bytes in-memory.
    alpr::AlprResults results = openalpr->recognize(imagelocation);
    for (int i = 0; i < results.plates.size(); i++)
    {
        alpr::AlprPlateResult plate = results.plates[i];
//        std::cout << "plate" << i << ": " << plate.topNPlates.size() << " results" << std::endl;
        *bestPlate = results.plates[i].bestPlate.characters+"\n";
        if(bestPlate){
            platerecimagepath.push_back(imagelocation);
            platerecimagepath.push_back(*bestPlate);
        }

        for (auto k = 0; k < plate.topNPlates.size(); k++)
        {
            alpr::AlprPlate candidate = plate.topNPlates[k];
//            std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence;
//            std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;
//            std::cout << "Best plate result" << i << ": " << results.plates[i].bestPlate.characters << " Results" << std::endl;
        }
    }

//    std::cout << "Best Plate "<< *bestPlate << endl;
    QString qstr = QString::fromStdString(*bestPlate);
    delete openalpr;
//    emit imagePath(path);
    emit imagePath(qstr);
//    emit imagePath(platerecimagepath);
//    return platerecimagepath;
}