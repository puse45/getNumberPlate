#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;


class Car {
private:
	Rect rect;
public:
	bool counted;
	int frameOffset;
	Car(Rect rect) {
		this->rect = rect;
		this->frameOffset = 0;
		this->counted = false;
	}
	
	inline double distanceBetweenPoints(Point a, Point b) {
		return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
	}
	
	inline void updatePosition(Point position) {
		rect.x = position.x;
		rect.y = position.y;
	}

 	inline Point getPosition() {
		return Point(rect.x, rect.y);
	}

	bool onTheLine(Point start, Point end) {
		if (getPosition().y < start.y + 17 && getPosition().y > start.y - 17)
		return true;
		else return false;
	}
	inline bool wasCounted() {
		return counted;
	}
};
