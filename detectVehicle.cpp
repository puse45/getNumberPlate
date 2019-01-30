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
//    QString currentPath = "/home/pinje/project/CountingCars/";
    string croppedImageDir = "/images";
    string savingImageDir = currentPath.toStdString()+ croppedImageDir;
    QDir dir(QString::fromStdString(savingImageDir));
    dir.removeRecursively();
//    connect(&recognitionLicense,&licenseRecognition::imagePath,this,&DetectVehicle::getLicensePlate);
//    connect(this,&DetectVehicle::on_stop,&recognitionLicense,&licenseRecognition::stop);
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
    emit on_stop();
    mStop = true;
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
//        cout<<"ttttttttttttttttttttttttttttttttttttttttttttt"<<endl;
//        cv::Mat croppedRef(frame, rect);
//        cv::Mat cropped;
//// Copy the data into new matrix
//        croppedRef.copyTo(cropped);
////        cv::imshow( "Original Image",  cropped);
//        string s =  saveDir+std::to_string(random_integer)+extension;
//        imwrite(s , cropped );
////        cout << "From function drawCarsContours " <<rect << endl;
//        rectangle(frame, rect, Scalar(69, 0, 225));
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
//    std::thread detThread(&licenseRecognition::test,&recognitionLicense,"");
//    std::thread detThread(&licenseRecognition::test,&recognitionLicense,"");
    Mat frame;
    Mat nextFrame;

    vector<Car> cars;
    vector<Car> previousCars;
    VideoCapture cap;

//    cap.open("/home/geoswift1/Videos/crop_cars/MOVA0089.avi");
//    cap.open("/home/geoswift1/Videos/crop_cars/1min.mp4");
    cap.open(videoPath);
    cap.read(frame);



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
//                recognitionLicense.mutex_.lock();
//                recognitionLicense.start(detectedImagePath);
//                recognitionLicense.test(detectedImagePath);
//                recognitionLicense.mutex_.unlock();
//                QFuture<void> test = QtConcurrent::run(&this->myjob,&licenseRecognition::start,QString("/home/pinje/project/CountingCars/images/1681692777.jpg"));
//                QFuture<void> test = QtConcurrent::run(&this->recognitionLicense,&licenseRecognition::start,QString(detectedImagePath));

//                detThread.join();
//                delete recognitionLicense;

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
//    detThread.join();

    destroyAllWindows();
}

void DetectVehicle::getLicensePlate(QString path ,QString plate) {
//    cout << plate.size();
    if(plate!=""){
        qDebug() << "Image path " << path << "Plate recoginised " << plate ;
    }

}

void DetectVehicle::startDetect()
{
//    VideoCapture cap;


//    cap.read(frame);
//    imshow("window2", frame);
    while(1)
    {
       cout << "detecting..." << endl;
//        frame = current_frame_.clone();
//        if(!frame.empty()){
//            imshow("window2", frame);
//        }
//        mutex_.lock();


//        mutex_.unlock();
        if(mStop){
            break;
        }
    }


}
void DetectVehicle::Vehicle_Recogniton(){

    while (!mStop) {
        cout << "Vehicle recognition set...." << endl;
//        QString imgpath = detectedImagePath;
//        qDebug() << imgpath << endl;
//
//        QFuture<void> test = QtConcurrent::run(&this->recognitionLicense, &licenseRecognition::start, QString(imgpath));
//        cv::Mat resized;
        mutex_.lock();
        Mat img=current_frame_.clone();
        mutex_.unlock();
        if(!img.empty()){
//            imshow("window54", img);
//            cv::resize(img, resized, cv::Size(800, 600));
            std::vector<unsigned char> buffer;
            cv::imencode(".bmp", img, buffer);
            std::vector<alpr::AlprRegionOfInterest> regionsOfInterest;

            regionsOfInterest.push_back(alpr::AlprRegionOfInterest(0,0, img.cols, img.rows));
            cout<<"Resize "<<img.elemSize();
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
    //                std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;
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
void DetectVehicle::Display_Res(){
//    int number = 0;
    while(!mStop){
//        cout<<"Number " <<number<<endl;
        mutex_.lock();
        Mat currframe=current_frame_.clone();
        Mat nxtframe=nextFrame.clone();
        mutex_.unlock();
        if(!currframe.empty() && !nxtframe.empty()) {
            vector<Rect> carsRect = drawCarsContours(currframe, contoursOfDetectedMove(currframe, nxtframe));
//            cout << carsRect.size() << endl;
//            cout << "NNNNNNNNNNNNNNNNNNNNNNNNNNNNN" << endl;
            for (Rect car : carsRect) {

                if (!matchToExistingCars(cars, car)) {

                    cars.push_back(Car(car));
                    //                cout << "Saved cropped vehicle images " << savedVehiclePath <<endl;
                }
            }
        }
//        number++;
//        if(!mStop){
//            break;
//        }
    }

}
vector<Rect> DetectVehicle::setFrame()
{


//
//    mutex_.lock();
//    current_frame_ = _new_frame.clone();
//    nextFrame = _new_next_frame.clone();
//    mutex_.unlock();


    while(true){
        mutex_.lock();
        Mat currframe=current_frame_.clone();
        Mat nxtframe=nextFrame.clone();


        if(!currframe.empty() && !nxtframe.empty()) {


        vector<Rect> carsRect = drawCarsContours(currframe, contoursOfDetectedMove(currframe, nxtframe));
        cout << carsRect.size() << endl;

//        for (Rect car : carsRect) {
//
//            if (!matchToExistingCars(cars, car)) {
//
//                cars.push_back(Car(car));
//                //                cout << "Saved cropped vehicle images " << savedVehiclePath <<endl;
//            } else {
//                previousCars.push_back(Car(car));
////            cout << "Previous car vector size "<< previousCars.size()<<endl;
//                string savedVehiclePath = cropVehicle(car, current_frame_);
////            cout << savedVehiclePath <<endl;
//                detectedImagePath = QString::fromStdString(savedVehiclePath);
////            recognitionLicense.mutex_.lock();
////                recognitionLicense.start(detectedImagePath);
////            recognitionLicense.test(detectedImagePath);
////            recognitionLicense.mutex_.unlock();
////                QFuture<void> test = QtConcurrent::run(&this->myjob,&licenseRecognition::start,QString("/home/pinje/project/CountingCars/images/1681692777.jpg"));
//                // QFuture<void> test = QtConcurrent::run(&this->recognitionLicense,&licenseRecognition::start,QString(detectedImagePath));
//
//            }
//            for (Car car : previousCars) {
//
////
//                if (car.onTheLine(Point(0, currframe.rows / 5 * 3), Point(currframe.cols, currframe.rows / 5 * 3)) &&
//                    !car.wasCounted()) {
//                    counter++;
//                    car.counted = true;
//                }
//                car.frameOffset++;
//            }

//        imshow("window2", current_frame_);

            return carsRect;
        //}
    }

    }




//    imshow("window2", current_frame_);
//    detThread.join();
}

void DetectVehicle::loop() {
//   ` for (auto x=0; x<10000; ++x){
//        cout << "Here is "<< x <<endl;
//    }
    while(!mStop){
//        cout << "looping " << endl;
//        if(mStop){
//            break;
//        }
    }

}

void DetectVehicle::stop() {
    qDebug()<<endl<<"Stopping detection "<<endl;
    mStop = true;

}