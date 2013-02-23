//============================================================================
// Name        : PhotometricStereoCpp.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "constant.h"
#include "ImageGrabber.h"
#include "LightingPanel.h"
#include "PhotometricStereo.h"
#include "ModelRenderer.h"
#include "InputAdjuster.h"
#include <GL/glfw.h>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Display.h"

using namespace std;
using namespace cv;

void doVideo(bool hardcode, bool saveCopy);
void doStatic(bool hardcode, bool saveCopy);
void calibrate();

int main(int argc, char *argv[]) {

	Display* display = Display::getInstance();

	try {
//		calibrate();
//		doVideo(false, false);
		doStatic(true, false); // hardcode, savecopy
	} catch (Exception& ex) {
		cerr << "Exception caught: " << ex.what() << endl;
	}

	destroyAllWindows();

	return 0;
}


void calibrate() {
	ImageGrabber grabber(false, false);
	PhotometricStereo ps;

	cout << "Static: waiting to start grab" << endl;

	for (int i=0; i<NUM_IMAGES*2; i++) {
		grabber.updateScreenAndCapture(false);
	}

	cout << "Static: Done grab" << endl;

	InputAdjuster adjuster;
	Mat adjustment;
	adjuster.createAdjustment(grabber.getImages(), adjustment);
	adjuster.getAdjustment(adjustment);
	imshow("tt0", adjustment.col(0).clone().reshape(0, CAPTURE_HEIGHT)/10);
	imshow("tt1", adjustment.col(1).clone().reshape(0, CAPTURE_HEIGHT)/10);
	imshow("tt2", adjustment.col(2).clone().reshape(0, CAPTURE_HEIGHT)/10);
	imshow("tt3", adjustment.col(3).clone().reshape(0, CAPTURE_HEIGHT)/10);


	cout << "Static: Done processing" << endl;
	while (true) {
		usleep(100*1000);
		if (!glfwGetWindowParam( GLFW_OPENED )) {
			return;
		}
	}
}

void doStatic(bool hardcode, bool saveCopy) {
	ImageGrabber grabber(hardcode, saveCopy);
	PhotometricStereo ps;

	Display* display = Display::getInstance();

	cout << "Static: waiting to start grab" << endl;

	for (int i=0; i<NUM_IMAGES*2; i++) {
//		if (waitKey(1) != -1) {
//			return;
//		}
		grabber.updateScreenAndCapture();
	}
//imshow("input0", grabber.getImages().col(0).clone().reshape(0, CAPTURE_HEIGHT));
//imshow("input1", grabber.getImages().col(1).clone().reshape(0, CAPTURE_HEIGHT));
//imshow("input2", grabber.getImages().col(2).clone().reshape(0, CAPTURE_HEIGHT));
//imshow("input3", grabber.getImages().col(3).clone().reshape(0, CAPTURE_HEIGHT));
//waitKey();
	cout << "Static: Done grab" << endl;

	Mat heightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
	Mat textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);

	ps.getHeightMap(grabber.getImages(), heightMap);
	grabber.getAvgImage(textureMap);

	cout << "Static: Done processing" << endl;

	while (true) {
		display->updateModel(heightMap.clone(), textureMap.clone());
		display->draw();
		usleep(100*1000);
		if (!glfwGetWindowParam( GLFW_OPENED )) {
			return;
		}
	}
}

void doVideo(bool hardcode, bool saveCopy) {
	ImageGrabber grabber(hardcode, saveCopy);
	PhotometricStereo ps;

	Display* display = Display::getInstance();

	for (int i=0; i<NUM_IMAGES*2; i++) { // get NUM_IMAGES real quick to fill up the grabber
		grabber.updateScreenAndCapture();
	}

	Mat heightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
	Mat textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
	while (true) {
#ifdef TIME_CHECK
		cout << "======= start ========" << endl;
		boost::system_time last = boost::get_system_time();
#endif
		grabber.updateScreenAndCapture();
#ifdef TIME_CHECK
		cout << "grabber: " << boost::get_system_time() - last << endl;

		last = boost::get_system_time();
#endif
		ps.getHeightMap(grabber.getImages(), heightMap);
#ifdef TIME_CHECK
		cout << "height: " << boost::get_system_time() - last << endl;

		last = boost::get_system_time();
#endif
		grabber.getAvgImage(textureMap);
#ifdef TIME_CHECK
		cout << "texture: " << boost::get_system_time() - last << endl;

		last = boost::get_system_time();
#endif
		display->updateModel(heightMap, textureMap.clone());
#ifdef TIME_CHECK
		cout << "model: " << boost::get_system_time() - last << endl;
#endif

		if (!glfwGetWindowParam( GLFW_OPENED )) {
			break;
		}
	}
}
