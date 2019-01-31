//
// Created by geoswift1 on 1/23/19.
//

#include "player.h"
#include "Car.cpp"

Player::Player(QObject *parent) {
    connect(this,&Player::on_stop_dv,&dv,&DetectVehicle::stop);
}

Player::~Player() {

}


void Player::startDetection(string videoPath) {
    Mat frame;
    Mat nextFrame;
    vector<Car> cars;
    vector<Car> previousCars;
    VideoCapture cap;
    cap.open(videoPath);
    cap.read(frame);

    if(!frame.empty()){
    std::thread RecogThread(&DetectVehicle::Vehicle_Recogniton,&dv);
    while (1) {
        int key = waitKey(15);
        cap.read(nextFrame);
        if(nextFrame.empty()){
            cout<<"End of video file"<<endl;
            emit on_stop_dv();
        }
        if (nextFrame.empty() || key == 'q')
            break;
        mutex_.lock();
        dv.Capture_Frame(frame,nextFrame);
        vector<Point> contour = dv.areaOfInterest();
        const cv::Point *pts = (const cv::Point*) Mat(contour).data;
        int npts = Mat(contour).rows;
        polylines(nextFrame, &pts,&npts, 1,
                  true, 			// draw closed contour (i.e. joint end to start)
                  Scalar(0,255,0),// colour RGB ordering (here = green)
                  2, 		        // line thickness
                  CV_AA, 0);
        mutex_.unlock();
        imshow("window", nextFrame);
        frame = nextFrame.clone();
        char c=(char)waitKey(25);
        if(c==27){
            emit on_stop_dv();
            break;
        }
    }
    RecogThread.join();
    destroyAllWindows();
    }
    else{
        cout<<"No video file found!!"<<endl;
    }
}