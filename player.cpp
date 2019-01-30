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

    std::thread detThread(&DetectVehicle::Display_Res,&dv);
    std::thread loopThread(&DetectVehicle::loop,&dv);
    std::thread RecogThread(&DetectVehicle::Vehicle_Recogniton,&dv);


    int counter = 0;

    while (1) {

        int key = waitKey(15);
        cap.read(nextFrame);

        if (nextFrame.empty() || key == 'q')
            break;
//        mutex_.lock();
//        dv.setFrame(frame,nextFrame);
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
//        dv.Vehicle_Recogniton();
        //vector<Rect> rects = dv.setFrame();
        mutex_.unlock();
//        cout <<  "Previous car "<< previousCars.size() << endl;
//        for(int x=0;x<rects.size();x++) {
//            cv::rectangle(frame, rects[x], Scalar(69, 0, 225));
//        }
        //rectangle( frame, Point( 115, 120 ), Point( 170, 150), Scalar( 100, 155, 25 ), -1, 8 );
//        for (Car car : previousCars) {
//            if(!car.wasCounted()){
//                cout << "Car frame :" << (frame.rows / 5 * 3) << endl;
//            }
//            if (car.onTheLine(Point(0, frame.rows / 5 * 3), Point(frame.cols, frame.rows / 5 * 3)) && !car.wasCounted()) {
//                //if (car.onTheLine(Point(0, frame.rows / 5 * 3), Point(frame.cols, frame.rows / 5 * 3)) && !car.wasCounted()) {
//                counter++;
//                car.counted = true;
//            }
//            car.frameOffset++;
//        }

//		showAmountOfCars(frame, counter);


//        mutex_.unlock();
        imshow("window", nextFrame);
        frame = nextFrame.clone();

        char c=(char)waitKey(25);
        if(c==27){
            emit on_stop_dv();
            break;
        }
        //previousCars.clear();

    }

    detThread.join();
    loopThread.join();
//    RecogThread.join();
    destroyAllWindows();
}