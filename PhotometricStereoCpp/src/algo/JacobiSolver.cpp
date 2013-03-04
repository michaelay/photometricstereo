/*
 * JacobiSolver.cpp
 *
 *  Created on: Aug 27, 2011
 *      Author: mistralay
 */

#include "JacobiSolver.h"
#include "../constant.h"
#include <iostream>
#include "boost/thread.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;

JacobiSolver::JacobiSolver() {
	// TODO Auto-generated constructor stub

}

JacobiSolver::~JacobiSolver() {
	// TODO Auto-generated destructor stub
}

void JacobiSolver::computeNormalDifferential(Mat normalVectors, int imageNumRow, Mat& xDifferentialMat, Mat& yDifferentialMat) {
	// TODO: do not clone, reshape twice
//	Mat zVector = normalVectors.col(0).clone().reshape(0, imageNumRow);
//	Mat xVector = normalVectors.col(1).clone().reshape(0, imageNumRow);
//	Mat yVector = normalVectors.col(2).clone().reshape(0, imageNumRow);
//
//	xDifferentialMat = xVector / zVector;
//	yDifferentialMat = yVector / zVector;

	xDifferentialMat = normalVectors.col(1) / normalVectors.col(0);
	xDifferentialMat = xDifferentialMat.reshape(0, imageNumRow);
//	xDifferentialMat.reshape(0, imageNumRow);

	yDifferentialMat = normalVectors.col(2) / normalVectors.col(0);
	yDifferentialMat = yDifferentialMat.reshape(0, imageNumRow);
//	yDifferentialMat.reshape(0, imageNumRow);
}


void JacobiSolver::normalToHeight(Mat normalVectors, int imageNumRow, Mat& heightMat) {
	Mat xDifferentialMat;
	Mat yDifferentialMat;

//	int imageNumCol = normalVectors.rows / imageNumRow;

#ifdef TIME_CHECK
	boost::system_time last = boost::get_system_time();
#endif
	computeNormalDifferential(normalVectors, imageNumRow, xDifferentialMat, yDifferentialMat);
#ifdef TIME_CHECK
	cout << "Jacobi normal diff: " << boost::get_system_time() - last << endl;
	last = boost::get_system_time();
#endif

#ifdef USE_GPU
	normalDifferentialToHeightGpu(xDifferentialMat, yDifferentialMat, heightMat, 0); // level 0 - last parameter
#else
	normalDifferentialToHeight(xDifferentialMat, yDifferentialMat, heightMat, 0); // level 0 - last parameter
#endif

//	resize(heightMat, heightMat, Size(imageNumCol, imageNumRow));


#ifdef TIME_CHECK
	cout << "Jacobi diff to height: " << boost::get_system_time() - last << endl;
#endif
}

// private
void JacobiSolver::computeDifferentialAdjust(Mat xDifferential, Mat yDifferential, Mat& adjust) {
	int numRow = xDifferential.rows;
	int numCol = xDifferential.cols;
	adjust = Mat::zeros(numRow, numCol, CV_32FC1);

	Mat yTail = yDifferential.rowRange(1, numRow);
	Mat yHead = yDifferential.rowRange(0, numRow-1);
	yTail -= yHead.clone();

	Mat xTail = xDifferential.colRange(1, numCol);
	Mat xHead = xDifferential.colRange(0, numCol-1);
	xTail -= xHead.clone();

	adjust += xDifferential;
	adjust += yDifferential;
	adjust /= 4.0;
}

void JacobiSolver::computeNeighborAdjust(Mat heightMat, Mat& adjust) {
	int numRow = heightMat.rows;
	int numCol = heightMat.cols;

	adjust = Mat::zeros(numRow, numCol, CV_32FC1);

	Mat heightSubMat;
	Mat adjustSubMat;

	heightSubMat = heightMat.rowRange(1, numRow);
	adjustSubMat = adjust.rowRange(0, numRow-1);
	adjustSubMat += heightSubMat;

	heightSubMat = heightMat.rowRange(0, numRow-1);
	adjustSubMat = adjust.rowRange(1, numRow);
	adjustSubMat += heightSubMat;

	heightSubMat = heightMat.colRange(1, numCol);
	adjustSubMat = adjust.colRange(0, numCol-1);
	adjustSubMat += heightSubMat;

	heightSubMat = heightMat.colRange(0, numCol-1);
	adjustSubMat = adjust.colRange(1, numCol);
	adjustSubMat += heightSubMat;

	adjust /= 4.0;
}

void JacobiSolver::normalDifferentialToHeight(Mat xDifferential, Mat yDifferential, Mat& heightMat, unsigned int level) {

	int numRow = heightMat.rows;
	int numCol = heightMat.cols;

#ifdef TIME_CHECK
		boost::system_time last = boost::get_system_time();
#endif

	// Pyramid up and down for faster processing
	// http://docs.opencv.org/doc/tutorials/imgproc/pyramids/pyramids.html
	if (heightMat.rows > MIN_ROW_TO_SCALE && heightMat.cols > MIN_ROW_TO_SCALE) { // perform on scaled down version
		Mat coarseX;
		Mat coarseY;
		Mat coarseHeight;

		pyrDown(heightMat, coarseHeight, Size(heightMat.cols/2, heightMat.rows/2));
		pyrDown(xDifferential, coarseX, Size(xDifferential.cols/2, xDifferential.rows/2));
		pyrDown(yDifferential, coarseY, Size(yDifferential.cols/2, yDifferential.rows/2));
		xDifferential *= 2;
		yDifferential *= 2;

		normalDifferentialToHeight(coarseX, coarseY, coarseHeight, ++level);

//		pyrUp(coarseHeight, heightMat, Size(coarseHeight.cols*2, coarseHeight.rows*2));
		pyrUp(coarseHeight, heightMat, Size(numCol, numRow));
	}

#ifdef TIME_CHECK
		cout << "js scaledown: " << boost::get_system_time() - last << endl;
		last = boost::get_system_time();
#endif

	// run the actual gauss seidel algorithm
	Mat pqAdjust;
	computeDifferentialAdjust(xDifferential, yDifferential, pqAdjust);

#ifdef TIME_CHECK
		cout << "js diff adjust: " << boost::get_system_time() - last << endl;
		last = boost::get_system_time();
#endif

	int k=0;
	for (k=0; k<MAX_NUM_ITERATION; k++) {
		bool hasChanged = false;

		Mat oldHeight = heightMat.clone();

		Mat neightborAdjust;
		computeNeighborAdjust(heightMat, neightborAdjust);

//#ifdef TIME_CHECK
//		cout << "js loop 2: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif
		heightMat = neightborAdjust + pqAdjust;

		// convergence check
		Mat absDiff;
		absdiff(heightMat, oldHeight, absDiff);
		Point maxPoint;
		Point minPoint;
		double min;
		double max;
		minMaxLoc(absDiff, &min, &max, &minPoint, &maxPoint);
		if (max > GAUSS_SEIDEL_THRESHOLD) {
			hasChanged = true;
		}
		if (!hasChanged) {
			break;
		}
//#ifdef TIME_CHECK
//		cout << "js loop 5: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif


	}

#ifdef TIME_CHECK
		cout << "js iteration total: " << boost::get_system_time() - last << endl;
		last = boost::get_system_time();
#endif

	std::cout << "num iterations: " << k << std::endl;

}

//void JacobiSolver::normalDifferentialToHeightGpu(Mat xDifferential, Mat yDifferential, Mat& heightMat, unsigned int level) {
//	int numRow = heightMat.rows;
//	int numCol = heightMat.cols;
//
//	GpuMatPtr xDiffGpuMatPtr = getXDiffGpuMat(level);
//	GpuMatPtr yDiffGpuMatPtr = getYDiffGpuMat(level);
//	GpuMatPtr heightGpuMatPtr = getHeightGpuMat(level, numRow, numCol, CV_32FC1);
//
//	xDiffGpuMatPtr->upload(xDifferential);
//	yDiffGpuMatPtr->upload(yDifferential);
////	*heightGpuMatPtr = heightMat;
//
//	normalDifferentialToHeightGpuHelper(xDiffGpuMatPtr, yDiffGpuMatPtr, heightGpuMatPtr, level);
//
//	heightGpuMatPtr->download(heightMat);
//}
//
//void JacobiSolver::normalDifferentialToHeightGpuHelper(GpuMatPtr xDifferential, GpuMatPtr yDifferential, GpuMatPtr heightMat, unsigned int level) {
//
//	int numRow = heightMat->rows;
//	int numCol = heightMat->cols;
//
//#ifdef TIME_CHECK
//		boost::system_time last = boost::get_system_time();
//#endif
//
//	if (numRow > MIN_ROW_TO_SCALE && numCol > MIN_ROW_TO_SCALE) { // perform on scaled down version
//		GpuMatPtr coarseX = getXDiffGpuMat(level+1);
//		GpuMatPtr coarseY = getXDiffGpuMat(level+1);
//		GpuMatPtr coarseHeight = getHeightGpuMat(level+1, numRow/2, numCol/2, CV_32FC1);
////		Mat coarseX;
////		Mat coarseY;
////		Mat coarseHeight;
//
////		gpu::pyrDown(*xDifferential, *coarseX);
////		gpu::pyrDown(*yDifferential, *coarseY);
////		gpu::pyrDown(*heightMat, *coarseHeight);
//		gpu::resize(*xDifferential, *coarseX, Size(numCol/2, numRow/2));
//		gpu::resize(*yDifferential, *coarseY, Size(numCol/2, numRow/2));
//		gpu::resize(*heightMat, *coarseHeight, Size(numCol/2, numRow/2));
//
////		scaleDownDifferentialAndHeight(xDifferential, yDifferential, heightMat, coarseX, coarseY, coarseHeight);
//		normalDifferentialToHeightGpuHelper(coarseX, coarseY, coarseHeight, level+1);
//
//		gpu::resize(*coarseHeight, *heightMat, Size(numCol, numRow));
//	}
//
//#ifdef TIME_CHECK
//		cout << "js scaledown: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif
//
//	GpuMatPtr heightGpuMatPtr = getHeightGpuMat(level, numRow, numCol, CV_32FC1);
//	GpuMatPtr pgAdjustGpuPtr = getPQAdjustGpuMat(level, numRow, numCol, CV_32FC1);
//	GpuMatPtr oldHeightGpuMat = getOldHeightGpuMat(level);
//	GpuMatPtr neightborAdjustGpuPtr = getNeighborGpuMat(level, numRow, numCol, CV_32FC1);
//	GpuMatPtr absDiffGpuMat = getDiffGpuMat(level);
//
////	heightGpuMatPtr->upload(heightMat);
//
//	// run the actual gauss seidel algorithm
////	Mat pqAdjust;
//	computeDifferentialAdjust(*xDifferential, *yDifferential, *pgAdjustGpuPtr);
////	pgAdjustGpuPtr->upload(pqAdjust);
//
//#ifdef TIME_CHECK
//		cout << "js diff adjust: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif
//
////#ifdef TIME_CHECK
////		boost::system_time neighbor_start = boost::get_system_time();
////		boost::system_time add_start = boost::get_system_time();
////		boost::system_time neighbor_total = 0;
////		boost::system_time add_total = 0;
////#endif
//
//
//	int k=0;
//	for (k=0; k<MAX_NUM_ITERATION; k++) {
//		bool hasChanged = false;
//
//		*oldHeightGpuMat = heightGpuMatPtr->clone();
//		computeNeighborAdjust(*heightGpuMatPtr, *neightborAdjustGpuPtr);
//
//		gpu::add(*neightborAdjustGpuPtr, *pgAdjustGpuPtr, *heightGpuMatPtr);
//
//		// convergence check
//		absdiff(*heightGpuMatPtr, *oldHeightGpuMat, *absDiffGpuMat);
//		Point maxPoint;
//		Point minPoint;
//		double min;
//		double max;
//		minMaxLoc(*absDiffGpuMat, &min, &max, &minPoint, &maxPoint);
//		if (max > GAUSS_SEIDEL_THRESHOLD) {
//			hasChanged = true;
//		}
//		if (!hasChanged) {
//			break;
//		}
//	}
////	heightGpuMatPtr->download(heightMat);
//
//#ifdef TIME_CHECK
//		cout << "js iteration total: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif
//
//	std::cout << "num iterations: " << k << std::endl;
//
//}
//
//
//
//// GPU manipulation
//GpuMatPtr JacobiSolver::getHeightGpuMat(unsigned int level, int numRow, int numCol, int type) {
//	if (heightGpuMatVector.size() <= level) {
//		heightGpuMatVector.resize(level+1);
//	}
//	if (heightGpuMatVector.at(level).empty()) {
//		heightGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat(numRow, numCol, type));
//		heightGpuMatVector.at(level)->setTo(0.0);
//	}
//	return heightGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getNeighborGpuMat(unsigned int level, int numRow, int numCol, int type) {
//	if (neighborGpuMatVector.size() <= level) {
//		neighborGpuMatVector.resize(level+1);
//	}
//	if (neighborGpuMatVector.at(level).empty()) {
//		neighborGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat(numRow, numCol, type));
//	}
//	return neighborGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getPQAdjustGpuMat(unsigned int level, int numRow, int numCol, int type) {
//	if (pqAdjustGpuMatVector.size() <= level) {
//		pqAdjustGpuMatVector.resize(level+1);
//	}
//	if (pqAdjustGpuMatVector.at(level).empty()) {
//		pqAdjustGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat(numRow, numCol, type));
//	}
//	return pqAdjustGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getOldHeightGpuMat(unsigned int level) {
//	if (oldHeightGpuMatVector.size() <= level) {
//		oldHeightGpuMatVector.resize(level+1);
//	}
//	if (oldHeightGpuMatVector.at(level).empty()) {
//		oldHeightGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat());
//	}
//	return oldHeightGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getDiffGpuMat(unsigned int level) {
//	if (diffGpuMatVector.size() <= level) {
//		diffGpuMatVector.resize(level+1);
//	}
//	if (diffGpuMatVector.at(level).empty()) {
//		diffGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat());
//	}
//	return diffGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getXDiffGpuMat(unsigned int level) {
//	if (xdiffGpuMatVector.size() <= level) {
//		xdiffGpuMatVector.resize(level+1);
//	}
//	if (xdiffGpuMatVector.at(level).empty()) {
//		xdiffGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat());
//	}
//	return xdiffGpuMatVector.at(level);
//}
//GpuMatPtr JacobiSolver::getYDiffGpuMat(unsigned int level) {
//	if (ydiffGpuMatVector.size() <= level) {
//		ydiffGpuMatVector.resize(level+1);
//	}
//	if (ydiffGpuMatVector.at(level).empty()) {
//		ydiffGpuMatVector.at(level) = GpuMatPtr(new gpu::GpuMat());
//	}
//	return ydiffGpuMatVector.at(level);
//}
//
//void JacobiSolver::pyramidDownGpu(GpuMatPtr up, GpuMatPtr down) {
////	int row = down->rows;
////	int col = down->cols;
////	for (int r=0; r<row; r++) {
////		for (int c=0; c<col; c++) {
////			down->
////		}
////	}
//}
//void JacobiSolver::pyramidUpGpu(GpuMatPtr down, GpuMatPtr up) {
//
//}

