//
// Created by pinje on 10/10/18.
//

#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrent>
#include "detectVehicle.h"
#include "Car.cpp"

using namespace std;
using namespace cv;


DetectVehicle::DetectVehicle(QObject *parent) {
    QString currentPath = DetectVehicle::imagePath();
//    QString currentPath = "/home/pinje/project/CountingCars/";
    string croppedImageDir = "/images";
    string savingImageDir = currentPath.toStdString()+ croppedImageDir;
    QDir dir(QString::fromStdString(savingImageDir));
    dir.removeRecursively();
    connect(&recognitionLicense,&licenseRecognition::imagePath,this,&DetectVehicle::getLicensePlate);
    connect(this,&DetectVehicle::on_stop,&recognitionLicense,&licenseRecognition::stop);
}

DetectVehicle::~DetectVehicle() {
    emit on_stop();
}

bool isCorectMatch(vector<Point> contour,Rect boundingRect) {
    double ratio = (double)boundingRect.width / (double)boundingRect.height;
    return(boundingRect.area() > 400 &&
           boundingRect.width  > 30  &&
           boundingRect.height > 30  &&
           ratio > 0.2 && ratio < 4.0 &&
           contourArea(contour) / (double)boundingRect.area() > 0.5
    );
}

vector<Rect>drawCarsContours(Mat& frame, vector<vector<Point>> contours) {
    vector<vector<Point>> hull(contours.size());
    Mat hullsFrame(frame.size(),CV_8UC1,Scalar(0,0,0));

    for (int i = 0;i < contours.size();i++) {
        convexHull(contours[i], hull[i]);
        Rect rect = boundingRect(contours[i]);

        if (isCorectMatch(hull[i], rect)) {
            drawContours(hullsFrame, hull, i, Scalar(255, 255, 255), -1);

        }
    }

    vector<vector<Point>> goodMatchsContours;
    vector<Rect> goodMatchRects;
    string extension = ".jpg";
    string saveDir = "/home/geoswift1/CLionProjects/getNumberPlate/images/";
    int random_integer = rand();
    findContours(hullsFrame, goodMatchsContours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    for (int i = 0;i < goodMatchsContours.size();i++) {
        Rect rect = boundingRect(goodMatchsContours[i]);
        goodMatchRects.push_back(rect);
        cv::Mat croppedRef(frame, rect);
        cv::Mat cropped;
// Copy the data into new matrix
        croppedRef.copyTo(cropped);
//        cv::imshow( "Original Image",  cropped);
        string s =  saveDir+std::to_string(random_integer)+extension;
        imwrite(s , cropped );
//        cout << "From function drawCarsContours " <<rect << endl;
        rectangle(frame, rect, Scalar(69, 0, 225));
//        imwrite(s,cropped );
    }
    //imshow("hull", hullsFrame);

    return goodMatchRects;
}

inline void createCountingLine(Mat& frame) {

    line(frame, Point(0, frame.rows / 5*3),Point(frame.cols,frame.rows/5*3),Scalar(0,0,0),4);
}

vector<vector<Point>> contoursOfDetectedMove(Mat& frame, Mat& nextFrame) {
    Mat grayFrame;
    Mat nextGrayFrame;
    Mat diffResult;

    Mat structuringElement = getStructuringElement(MORPH_RECT, Size(5, 5));

    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
    cvtColor(nextFrame, nextGrayFrame, COLOR_BGR2GRAY);

    GaussianBlur(grayFrame, grayFrame, Size(5, 5), 0, 0);
    GaussianBlur(nextGrayFrame, nextGrayFrame, Size(5, 5), 0, 0);

    absdiff(grayFrame, nextGrayFrame, diffResult);
    threshold(diffResult, diffResult, 30, 255, CV_THRESH_BINARY);

    for (int i = 0;i < 2;i++) {
        dilate(diffResult, diffResult, structuringElement);
        dilate(diffResult, diffResult, structuringElement);
        erode(diffResult, diffResult, structuringElement);
    }

//	imshow("diff", diffResult);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(diffResult, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    return contours;
}

inline double distanceBetweenPoints(Point a, Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}


bool matchToExistingCars(vector<Car> cars,Rect currentCar) {
//	cout << cars.size() << endl;
    for (Car car : cars) {
        if (distanceBetweenPoints(car.getPosition(),Point(currentCar.x,currentCar.y)) < 45.0) {
//			cout << car.getPosition() << " " << Point(currentCar.x, currentCar.y)<<endl;
            car.updatePosition(Point(currentCar.x, currentCar.y));
            return true;
        }
    }
    return false;
}

inline void showAmountOfCars(Mat& frame, int amountOfCars) {
    putText(frame, to_string(amountOfCars), Point(frame.cols/2-20, frame.rows / 5 * 3 - 20), FONT_HERSHEY_SIMPLEX, 4, (0, 0, 0),3);
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
//        cout << cars << endl;
//        cout << s << endl;
}


void DetectVehicle::startDetection(string videoPath) {
    Mat frame;
    Mat nextFrame;

    vector<Car> cars;
    vector<Car> previousCars;
    VideoCapture cap;

//    cap.open("/home/geoswift1/Videos/crop_cars/MOVA0089.avi");
//    cap.open("/home/geoswift1/Videos/crop_cars/1min.mp4");
    cap.open(videoPath);
    cap.read(frame);

    int counter = 0;

    while (1) {

        int key = waitKey(15);
        cap.read(nextFrame);
        if (nextFrame.empty() || key == 'q')
            break;

        vector<Rect> carsRect = drawCarsContours(frame, contoursOfDetectedMove(frame, nextFrame));

//		createCountingLine(frame);


        for (Rect car : carsRect) {
//            cout << car <<endl;
            if (!matchToExistingCars(cars, car)){
                cars.push_back(Car(car));
               //                cout << "Saved cropped vehicle images " << savedVehiclePath <<endl;
            }

            else {
                previousCars.push_back(Car(car));
                string savedVehiclePath = cropVehicle(car,frame);
                QString detectedImagePath = QString::fromStdString(savedVehiclePath);
//                QFuture<void> test = QtConcurrent::run(&this->myjob,&licenseRecognition::start,QString("/home/pinje/project/CountingCars/images/1681692777.jpg"));
                QFuture<void> test = QtConcurrent::run(&this->recognitionLicense,&licenseRecognition::start,QString(detectedImagePath));
            }
        }

        for (Car car : previousCars) {
            if (car.onTheLine(Point(0, frame.rows / 5 * 3), Point(frame.cols, frame.rows / 5 * 3)) && !car.wasCounted()) {
                counter++;
                car.counted = true;
            }
            car.frameOffset++;
        }

//		showAmountOfCars(frame, counter);
//		cout << previousCars.size() << endl;
        previousCars.clear();

        imshow("window", frame);
        frame = nextFrame.clone();
        char c=(char)waitKey(25);
        if(c==27){
            emit on_stop();
            break;
        }

    }

    destroyAllWindows();
}

void DetectVehicle::getLicensePlate(QString path ,QString plate) {
//    cout << plate.size();
    if(plate!=""){
        qDebug() << "Image path " << path << "Plate recoginised " << plate ;
    }

}