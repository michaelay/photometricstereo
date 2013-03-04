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

void doStatic(ImageGrabber* grabber, PhotometricStereo* ps, bool hardcode);
void doGenerate(int argc, char** argv, string prefix);


ImageGrabber* grabber;
PhotometricStereo* ps;

void callback() {
	try {
		doStatic(grabber, ps, true); // hardcode
		//		doGenerate(argc, argv, "ss_");
	} catch (Exception& ex) {
		cerr << "Exception caught: " << ex.what() << endl;
	}
}

//Mat heightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
Mat textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);

int main(int argc, char *argv[]) {
	ImageGrabber::setHardcode(true);
	grabber = ImageGrabber::getInstance();
	ps = new PhotometricStereo();

	// pass command line parameters to GlutDisplay
	GlutDisplay::setArg(argc, argv);
	GlutDisplay* display = GlutDisplay::getInstance();

	display->setHeightMap(ps->getHeightMap());
	display->setTextureMap(textureMap);
	display->setDisplayMode(GlutDisplay::MANUAL);
	display->setCallback(callback);

	display->start();

	delete ps;
	return 0;
}

void doGenerate(int argc, char** argv, string prefix) {
	SampleImageGenerator* generator = new SampleImageGenerator();
	generator->generateImage(argc, argv, prefix);
	delete generator;
}

void doStatic(ImageGrabber* grabber, PhotometricStereo* ps, bool hardcode) {
	cout << "Static: waiting to start grab" << endl;
	grabber->updateScreenAndCapture();
	cout << "Static: Done grab" << endl;
	ps->solve(grabber->getImages());
//	grabber.getAvgImage(textureMap); // TODO: get albedo map from ps object instead
	cout << "Static: Done processing" << endl;
}
