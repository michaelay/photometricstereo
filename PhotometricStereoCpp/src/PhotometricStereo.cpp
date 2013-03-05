/*
 * PhotometricStereo.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#include "PhotometricStereo.h"
#include "constant.h"
#include "algo/GaussSeidelSolver.h"
#include "algo/JacobiSolver.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "ui/Display.h"
#include "boost/thread.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


using namespace cv;
using namespace std;

PhotometricStereo::PhotometricStereo() {
	// openGL coordinate x -> right, y -> up, z -> outward (from screen to eye)
	// opencv coordinate x -> right, y -> down, z -> inward (from eye to screen)
	// http://stackoverflow.com/questions/9081900/reference-coordinate-system-changes-between-opencv-opengl-and-android-sensor
	// opencv coordinate system
	mExpectedLvalues[0][0] = 0.0;
	mExpectedLvalues[0][1] = -1.0;
	mExpectedLvalues[1][0] = 0.0;
	mExpectedLvalues[1][1] = 1.0;
	mExpectedLvalues[2][0] = 1.0;
	mExpectedLvalues[2][1] = 0.0;
	mExpectedLvalues[3][0] = -1.0;
	mExpectedLvalues[3][1] = 0.0;
	mExpectedLz = -1; // not used yet
	mExpectedL = Mat(4, 2, CV_32FC1, mExpectedLvalues);

	mHeightMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC1);
}

PhotometricStereo::~PhotometricStereo() {
}

Mat
PhotometricStereo::getNormalMap() {
	return mNormalMap;
}

Mat
PhotometricStereo::getHeightMap() {
	return mHeightMap;
}

void
PhotometricStereo::solve(Mat images) {

	boost::system_time last = boost::get_system_time();

//	Mat roi;
//	getROI(images, roi);
//	cout << "ps getroi: " << boost::get_system_time() - last << endl;
	// threshold to zero, see http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
	threshold(images, images, ROI_THRESHOLD, 0.0 ,3);

	last = boost::get_system_time();
	getNormal(images);
	cout << "ps normals: " << boost::get_system_time() - last << endl;

	last = boost::get_system_time();
	getHeight();
	cout << "ps height: " << boost::get_system_time() - last << endl;

	//	mVideoWriter.open("height.avi",CV_FOURCC('X','V','I','D'), 10, cv::Size(CAPTURE_WIDTH, CAPTURE_HEIGHT),1);
//	getTextureMap(images, textureMap);
}

//void PhotometricStereo::getTextureMap(Mat images, Mat textureMap) {
//	textureMap = Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_32FC3);
//	Mat col;
//	for (int i=0; i<NUM_IMAGES;  i++) {
//		col = images.col(i).clone();
//		textureMap += col.reshape(0, CAPTURE_HEIGHT);
//	}
//	textureMap /= NUM_IMAGES;
//}

// private
//void PhotometricStereo::getROI(Mat images, Mat& roi) {
////	Mat roi = Mat::zeros(CAPTURE_HEIGHT * CAPTURE_WIDTH, 1, CV_8UC1);
//	Mat columnSum= Mat::zeros(CAPTURE_HEIGHT * CAPTURE_WIDTH, 1, CV_32FC1);
//
//	int numImages = images.cols;
//	for (int i=0; i<numImages; i++) {
//		columnSum += images.col(i);
//	}
//	columnSum /= numImages;
//
////	Mat ret;
//	compare(columnSum, ROI_THRESHOLD, roi, CMP_GE);
//	if (SHOW_ROI) {
//		showROI(WINDOW_DEBUG, roi);
//	}
////	return ret;
//}

void PhotometricStereo::getNormal(Mat images) {

	Mat A = images.clone(); // input is Grabber::image, clone one for debug purpose
	Mat At = A.t();
	Mat ATA = At * A;

	Mat eigenvalues; // eigenvalue
	Mat leftSingular; // left singular
	Mat rightSingular; // right singular
	SVD::compute(ATA, eigenvalues, leftSingular, rightSingular);

//	cout << "S" << endl;
//	cout << leftSingular << endl;
//	cout << "L:" << endl;
//	cout << rightSingular << endl;
//	cout << "eigenvalues" << endl;
//	cout << eigenvalues << endl;

	// L = U * first three rows of T
	Mat L = rightSingular.rowRange(0, 3);
//	cout << "L1:" << endl;
//	cout << L << endl;

	// resolve linear ambiguity
	Mat permutation = rightSingular.rowRange(1,3).clone();
	normalize(permutation.row(0), permutation.row(0));
	normalize(permutation.row(1), permutation.row(1));
	permutation = permutation * mExpectedL;

	Mat permutationL = Mat::zeros(3,3,CV_32FC1);
	permutationL.at<float>(0,0) = 1.0;
	permutationL.at<float>(1,1) = round(permutation.at<float>(0,0));
	permutationL.at<float>(1,2) = round(permutation.at<float>(0,1));
	permutationL.at<float>(2,1) = round(permutation.at<float>(1,0));
	permutationL.at<float>(2,2) = round(permutation.at<float>(1,1));
	cout << permutationL << endl;

	L = permutationL * L;

	cout << "permutated L:" << endl;
	cout << L << endl;

//	Mat normals;
	mNormalMap = (L * A.t()).t();
//	mNormalMap = mNormalMap.t();

//	// strip by roi
//	Mat flipRoi;
//	bitwise_not(roi, flipRoi);
//	for (int i=0; i<3; i++) {
//		mNormalMap.col(i).setTo(0, flipRoi);
//	}

	// normalize
	Mat row;
	for (int j=0; j < mNormalMap.size.p[0]; j++) {
		normalize(mNormalMap.row(j), mNormalMap.row(j));
	}

//	cout << "dump normals vertical" << endl;
//	for (int nx = 0; nx < 320*240; nx += 320 * 2) {
//		cout << mNormalMap.row(nx + 160) << endl;
//	}
//	cout << "dump normals horizontal" << endl;
//	for (int ny = 320*240/2 - 120 -1; ny < 320*240/2 + 120; ny++) {
//		cout << mNormalMap.row(ny + 120) << endl;
//	}

//	if (SHOW_NORMAL) {
//		showNormal(WINDOW_DEBUG, normals);
//	}
}

void
PhotometricStereo::getHeight() {
//	solver.normalToHeight(mNormalMap, CAPTURE_HEIGHT, mHeightMap);
	solver.normalToHeightIterative(mNormalMap, CAPTURE_HEIGHT, mHeightMap);
	cout << "dump height vertical" << endl;
	cout << mHeightMap.col(160) << endl;
	cout << "dump height horizontal" << endl;
	cout << mHeightMap.row(120) << endl;
}

void PhotometricStereo::showNormal(const string& windowName, Mat normals) {
	// print normals debug
	Mat col = normals.col(0).clone();
	imshow(windowName, col.reshape(0, CAPTURE_HEIGHT));
#ifdef INTERACTIVE
	waitKey();
#endif

	col = normals.col(1).clone();
	imshow(windowName, col.reshape(0, CAPTURE_HEIGHT) + 0.5);
#ifdef INTERACTIVE
	waitKey();
#endif

	col = normals.col(2).clone();
	imshow(windowName, col.reshape(0, CAPTURE_HEIGHT) + 0.5);
#ifdef INTERACTIVE
	waitKey();
#endif
}

//void PhotometricStereo::showHeightMap(const string& windowName, Mat heightMat) {
//	double minVal;
//	double maxVal;
//	minMaxLoc(heightMat, &minVal, &maxVal);
//	Mat clone = heightMat.clone();
//	clone -= minVal;
//	clone /= (maxVal - minVal); // range from 0 to 1
////	clone *= 255;
////	imshow(windowName, clone);
//
//	Display* display = Display::getInstance();
//	display->updateHeightMap(clone);
////	display->drawHeightMap();
////	Display::getInstance()->updateHeightMap(clone);
////	Display::getInstance()->draw();
//
////	if (RECORD_VIDEO) {
////		mVideoWriter.write(clone);
////	}
//
//#ifdef INTERACTIVE
//	waitKey();
//#endif
//}

//void PhotometricStereo::showROI(const string& windowName, Mat roi) {
//	Mat clone = roi.clone();
//	clone *= 255;
//	imshow(WINDOW_DEBUG, clone.reshape(0, CAPTURE_HEIGHT));
//#ifdef INTERACTIVE
//	waitKey();
//#endif
//}
