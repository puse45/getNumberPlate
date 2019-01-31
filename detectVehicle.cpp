//
// Created by pinje on 10/10/18.
//

#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrent>
#include "detectVehicle.h"


using namespace std;
using namespace cv;


DetectVehicle::DetectVehicle(QObject *parent) {
    QString currentPath = DetectVehicle::imagePath();
    string croppedImageDir = "/images";
    string savingImageDir = currentPath.toStdString()+ croppedImageDir;
    QDir dir(QString::fromStdString(savingImageDir));
    dir.removeRecursively();
    openalpr = new alpr::Alpr("eu,vn2", "/etc/openalpr/openalpr.conf");
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
}

DetectVehicle::~DetectVehicle() {
    mStop = true;
}

QString DetectVehicle::imagePath() {
    QString path = QDir::currentPath();
    return path;
}

string cropVehicle(Rect cars, Mat& frame){
    QString currentPath = QDir::currentPath();
//    QString currentPath = "/home/pinje/project/CountingCars/";
    string croppedImageDir = "/images";
    string savingImageDir = currentPath.toStdString()+ croppedImageDir;
    QString savingPath = QString::fromStdString(savingImageDir);

    if(!QDir(savingPath).exists()){
        QDir().mkdir(savingPath);
    }

    cv::Mat croppedRef(frame, cars);
    cv::Mat cropped;
    // Copy the data into new matrix
    croppedRef.copyTo(cropped);
    //        cv::imshow( "Original Image",  cropped);
    string extension = ".jpg";
    int random_integer = rand();
    string s =  savingImageDir+"/"+std::to_string(random_integer)+extension;
    imwrite(s , cropped );
    return s;
}

void DetectVehicle::Vehicle_Recogniton(){

    while (!mStop) {
        mutex_.lock();
        Mat img=current_frame_.clone();
        mutex_.unlock();
        if(!img.empty()){
            std::vector<unsigned char> buffer;
            cv::imencode(".bmp", img, buffer);
            std::vector<alpr::AlprRegionOfInterest> regionsOfInterest;
            regionsOfInterest.push_back(alpr::AlprRegionOfInterest(0,0, img.cols, img.rows));
            alpr::AlprResults results;
            results = openalpr->recognize(img.data, img.elemSize(), img.cols, img.rows,regionsOfInterest);
            for (int i = 0; i < results.plates.size(); i++)
            {
                int x1 = results.plates[i].plate_points[0].x, y1 = results.plates[i].plate_points[0].y;
                int x2 = results.plates[i].plate_points[1].x, y2 = results.plates[i].plate_points[1].y;
                int x3 = results.plates[i].plate_points[2].x, y3 = results.plates[i].plate_points[2].y;
                int x4 = results.plates[i].plate_points[3].x, y4 = results.plates[i].plate_points[3].y;
                std::string platechar;
                platechar = results.plates[i].bestPlate.characters;
                platechar.erase(std::remove(platechar.begin(), platechar.end(), '\n'), platechar.end());

                alpr::AlprPlateResult plate = results.plates[i];
                vector<Point> contour;
                contour.push_back(Point(x1,y1));
                contour.push_back(Point(x2,y2));
                contour.push_back(Point(x3,y3));
                contour.push_back(Point(x4,y4));
                detectedPts = contour;
                const cv::Point *pts = (const cv::Point*) Mat(contour).data;
                int npts = Mat(contour).rows;
                polylines(img, &pts,&npts, 1,
                          true, 			// draw closed contour (i.e. joint end to start)
                          Scalar(0,255,0),// colour RGB ordering (here = green)
                          1, 		        // line thickness
                          CV_AA, 0);
//                std::cout << " Plate predicted : " << platenumberpredicted << "\t Confidence level : " << candidate.overall_confidence << std::endl;
                for (auto k = 0; k < plate.topNPlates.size(); k++)
                {
                    alpr::AlprPlate candidate = plate.topNPlates[k];
                    std::string platenumberpredicted;
                    platenumberpredicted = candidate.characters;
                    platenumberpredicted.erase(std::remove(platenumberpredicted.begin(), platenumberpredicted.end(), '\n'), platenumberpredicted.end());
                    std::cout << " Plate predicted : " << platenumberpredicted << "\t Confidence level : " << candidate.overall_confidence << std::endl;
                    std::cout << "Best plate result : " << platechar << " Results" << std::endl;
                }

            }
        }
    }
}

vector<Point> DetectVehicle::areaOfInterest(){
    vector<Point> pts=detectedPts;
    if(!pts.empty()){
        return pts;
    }
}

void DetectVehicle::Capture_Frame(const Mat& _new_frame, Mat& _new_next_frame)
{
    current_frame_ = _new_frame.clone();
    nextFrame = _new_next_frame.clone();
}


void DetectVehicle::stop() {
    qDebug()<<endl<<"Stopping detection "<<endl;
    mStop = true;
}