/*
 * PhotometricStereo.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#include "PhotometricStereo.h"
#include "constant.h"
#include "GaussSeidelSolver.h"
#include "JacobiSolver.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Display.h"
#include "boost/thread.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


using namespace cv;
using namespace std;

PhotometricStereo::PhotometricStereo() {
	mExpectedL = Mat(4, 2, CV_32FC1);
	mExpectedL.at<float>(0, 0) = 0;
	mExpectedL.at<float>(0, 1) = 1;
	mExpectedL.at<float>(1, 0) = 0;
	mExpectedL.at<float>(1, 1) = -1;
	mExpectedL.at<float>(2, 0) = 1;
	mExpectedL.at<float>(2, 1) = 0;
	mExpectedL.at<float>(3, 0) = -1;
	mExpectedL.at<float>(3, 1) = 0;
}

PhotometricStereo::~PhotometricStereo() {
	// TODO Auto-generated destructor stub
}

void PhotometricStereo::getHeightMap(Mat images, Mat heightMap) {

	boost::system_time last = boost::get_system_time();

	Mat roi;
	getROI(images, roi);
	cout << "ps getroi: " << boost::get_system_time() - last << endl;

	last = boost::get_system_time();
	Mat normals = getNormal(images, roi);
	cout << "ps normals: " << boost::get_system_time() - last << endl;

	last = boost::get_system_time();
	getHeight(normals, roi, heightMap);
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
void PhotometricStereo::getROI(Mat images, Mat& roi) {
//	Mat roi = Mat::zeros(CAPTURE_HEIGHT * CAPTURE_WIDTH, 1, CV_8UC1);
	Mat columnSum= Mat::zeros(CAPTURE_HEIGHT * CAPTURE_WIDTH, 1, CV_32FC1);

	int numImages = images.cols;
	for (int i=0; i<numImages; i++) {
		columnSum += images.col(i);
	}
	columnSum /= numImages;

//	Mat ret;
	compare(columnSum, ROI_THRESHOLD, roi, CMP_GE);
	if (SHOW_ROI) {
		showROI(WINDOW_DEBUG, roi);
	}
//	return ret;
}

Mat PhotometricStereo::getNormal(Mat images, Mat roi) {

	Mat A = images.clone(); // input is Grabber::image, clone one for debug purpose
	Mat At = A.t();
	Mat ATA = At * A;

	cout << "A:" << endl;
	for (int i = 32180; i < 32190; i++) {
		cout << A.row(i) << endl;
	}


	Mat eigenvalues; // eigenvalue
	Mat leftSingular; // left singular
	Mat rightSingular; // right singular
	SVD::compute(ATA, eigenvalues, leftSingular, rightSingular);

	// square root U
	Mat eigenvaluesRoot;
	sqrt(eigenvalues, eigenvaluesRoot);

	Mat eigenValuesRootMat = Mat::zeros(3, 3, CV_32FC1);
	eigenValuesRootMat.at<float>(0, 0) = eigenvaluesRoot.at<float>(0);
	eigenValuesRootMat.at<float>(1, 1) = eigenvaluesRoot.at<float>(1);
	eigenValuesRootMat.at<float>(2, 2) = eigenvaluesRoot.at<float>(2);

	cout << "S" << endl;
	cout << leftSingular << endl;
	cout << "L:" << endl;
	cout << rightSingular << endl;
	cout << "eigenvalues" << endl;
	cout << eigenvalues << endl;
	cout << "eignervalues root mat" << endl;
	cout << eigenValuesRootMat << endl;

	// L = U * first three rows of T
//	Mat L = eigenValuesRootMat * (rightSingular.rowRange(0, 3));
	Mat L = rightSingular.rowRange(0, 3);

//	Mat Llast2 = L.rowRange(1, 3);
//	Mat result = Llast2 * mExpectedL;
//
//	Mat M = Mat::zeros(3, 3, CV_32FC1);
//	bool allNegative = true;
//
//	// get first row of T, check if all negative
//	const float* firstRow = rightSingular.ptr<float>(0);
//	for (int i=0; i<rightSingular.cols; i++) {
//		if (firstRow[i] > 0) {
//			allNegative = false;
//			break;
//		}
//	}
//	if (allNegative) {
//		M.at<float>(0,0) = -1;
//	} else {
//		M.at<float>(0,0) = 1;
//	}
//
//	Mat resultAbs = abs(result);
//	double min;
//	double max;
//	Point maxLoc;
//	minMaxLoc(resultAbs, &min, &max, NULL, &maxLoc);
//
//	int maxRow = maxLoc.y;
//	int maxCol = maxLoc.x;
//
//	M.at<float>(maxRow+1, maxCol+1) = (result.at<float>(maxRow, maxCol) > 0) ? 1 : -1;
////      M.put(maxRow+1, maxCol+1, (ret.get(maxRow, maxCol) > 0) ? 1 : -1);
//
//	// the other one ( x or y )
//	int nextRow = (maxRow+1)%2;
//	int nextCol = (maxCol+1)%2;
//	M.at<float>(nextRow+1, nextCol+1) = (result.at<float>(nextRow, nextCol) > 0) ? 1: -1;
////	M.put(nextRow+1, nextCol+1, (ret.get(nextRow, nextCol) > 0) ? 1: -1);
//
//	M = M.t();
//
//
//	L = M*L;

	cout << "L':" << endl;
	cout << L << endl;

	Mat normals;
	normals = L * A.t();
	normals = normals.t();

//	// strip by roi
//	Mat flipRoi;
//	bitwise_not(roi, flipRoi);
//	for (int i=0; i<3; i++) {
//		normals.col(i).setTo(0, flipRoi);
//	}

	// normalize
	Mat row;
	for (int j=0; j < normals.size.p[0]; j++) {
		normalize(normals.row(j), normals.row(j));
	}

//	for (int i = 28800; i < 28810; i++) {
	cout << "normals:" << endl;
	for (int i = 32180; i < 32190; i++) {
		cout << normals.row(i) << endl;
	}

	if (SHOW_NORMAL) {
		showNormal(WINDOW_DEBUG, normals);
	}

	return normals;
}

void PhotometricStereo::getHeight(Mat normals, Mat roi, Mat heightMap) {
//	GaussSeidelSolver solver;
////	solver.normalToHeightGpu(normals, CAPTURE_HEIGHT, heightMap);
//	solver.normalToHeight(normals, CAPTURE_HEIGHT, heightMap);

	solver.normalToHeight(normals, CAPTURE_HEIGHT, heightMap);
	if (SHOW_HEIGHTMAP) {
		showHeightMap(WINDOW_DEBUG, heightMap);
	}
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

void PhotometricStereo::showHeightMap(const string& windowName, Mat heightMat) {
	double minVal;
	double maxVal;
	minMaxLoc(heightMat, &minVal, &maxVal);
	Mat clone = heightMat.clone();
	clone -= minVal;
	clone /= (maxVal - minVal); // range from 0 to 1
//	clone *= 255;
//	imshow(windowName, clone);

	Display* display = Display::getInstance();
	display->updateHeightMap(clone);
//	display->drawHeightMap();
//	Display::getInstance()->updateHeightMap(clone);
//	Display::getInstance()->draw();

//	if (RECORD_VIDEO) {
//		mVideoWriter.write(clone);
//	}

#ifdef INTERACTIVE
	waitKey();
#endif
}

void PhotometricStereo::showROI(const string& windowName, Mat roi) {
	Mat clone = roi.clone();
	clone *= 255;
	imshow(WINDOW_DEBUG, clone.reshape(0, CAPTURE_HEIGHT));
#ifdef INTERACTIVE
	waitKey();
#endif
}
