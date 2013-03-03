//============================================================================
// Name        : PhotometricStereoCpp.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <GL/glfw.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "constant.h"
#include "PhotometricStereo.h"

#include "ui/ImageGrabber.h"
#include "ui/Display.h"
#include "ui/GlutDisplay.h"

#include "sample/ImageGenerator.h"

using namespace std;
using namespace cv;

void doVideo(bool hardcode, bool saveCopy);
void doStatic(bool hardcode);
void doGenerate(int argc, char** argv, string prefix);


void callback() {
	try {
		//		doVideo(true, false);
		doStatic(true); // hardcode
		//		doGenerate(argc, argv, "ss_");
	} catch (Exception& ex) {
		cerr << "Exception caught: " << ex.what() << endl;
	}
}

Mat heightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
Mat textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);

int main(int argc, char *argv[]) {
	ImageGrabber::setHardcode(true);

	// pass command line parameters to GlutDisplay
	GlutDisplay::setArg(argc, argv);
	GlutDisplay* display = GlutDisplay::getInstance();

	display->setHeightMap(heightMap);
	display->setTextureMap(textureMap);
	display->setDisplayMode(GlutDisplay::MANUAL);
	display->setCallback(callback);

	display->start();
	return 0;
}

void doGenerate(int argc, char** argv, string prefix) {
	SampleImageGenerator* generator = new SampleImageGenerator();
	generator->generateImage(argc, argv, prefix);
	delete generator;
}

void doStatic(bool hardcode) {
	ImageGrabber* grabber = ImageGrabber::getInstance();
	PhotometricStereo ps;
	cout << "Static: waiting to start grab" << endl;
	grabber->updateScreenAndCapture();
	cout << "Static: Done grab" << endl;
	ps.getHeightMap(grabber->getImages(), heightMap);
//	grabber.getAvgImage(textureMap); // TODO: get albedo map from ps object instead
	cout << "Static: Done processing" << endl;
//	usleep(100*1000);
}

void doVideo(bool hardcode, bool saveCopy) {
	ImageGrabber* grabber = ImageGrabber::getInstance();
	PhotometricStereo ps;

	Display* display = Display::getInstance();


	for (int i=0; i<NUM_IMAGES*2; i++) { // get NUM_IMAGES real quick to fill up the grabber
		grabber->updateScreenAndCapture();
	}

	Mat heightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
	Mat textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
	while (true) {
#ifdef TIME_CHECK
		cout << "======= start ========" << endl;
		boost::system_time last = boost::get_system_time();
#endif
		grabber->updateScreenAndCapture();
#ifdef TIME_CHECK
		cout << "grabber: " << boost::get_system_time() - last << endl;

		last = boost::get_system_time();
#endif
		ps.getHeightMap(grabber->getImages(), heightMap);
#ifdef TIME_CHECK
		cout << "height: " << boost::get_system_time() - last << endl;

		last = boost::get_system_time();
#endif
		grabber->getAvgImage(textureMap);
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

