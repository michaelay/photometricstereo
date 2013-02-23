/*
 * LightingPanel.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#include <iostream>

#include "LightingPanel.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

LightingPanel::LightingPanel() {
	initDirectionMatList();
	showDirection(UP);
	cout << "Lighting panel constructor" << endl;
}

LightingPanel::~LightingPanel() {
}

void LightingPanel::showDirection(LightingPanelDirection dir) {
	imshow(WINDOW_LIGHTING, mDirectionMatList[dir]);
}

// private
void LightingPanel::initDirectionMatList() {
	mDirectionMatList.resize(NUM_DIRECTION);

	// up direction
	Mat upMat = Mat::zeros(ROW, COL, CV_8UC1); // 1 channel gray scale 8U
	Mat downMat = Mat::zeros(ROW, COL, CV_8UC1);
	Mat leftMat = Mat::zeros(ROW, COL, CV_8UC1);
	Mat rightMat = Mat::zeros(ROW, COL, CV_8UC1);

	rectangle(upMat, Point((COL-ROW)/2, 0), Point((COL+ROW)/2, ROW/2), Scalar::all(255.0), CV_FILLED);
	rectangle(downMat, Point((COL-ROW)/2, ROW/2), Point((COL+ROW)/2, ROW), Scalar::all(255.0), CV_FILLED);
	rectangle(leftMat, Point((COL-ROW)/2, 0), Point(COL/2, ROW), Scalar::all(255.0), CV_FILLED);
	rectangle(rightMat, Point(COL/2, 0), Point((COL+ROW)/2, ROW), Scalar::all(255.0), CV_FILLED);

	mDirectionMatList.at(UP) = upMat;
	mDirectionMatList.at(DOWN) = downMat;
	mDirectionMatList.at(LEFT) = leftMat;
	mDirectionMatList.at(RIGHT) = rightMat;
}
