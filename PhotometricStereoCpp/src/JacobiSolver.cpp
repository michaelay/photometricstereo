/*
 * JacobiSolver.cpp
 *
 *  Created on: Aug 27, 2011
 *      Author: mistralay
 */

#include "JacobiSolver.h"
#include "constant.h"
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
	Mat zVector = normalVectors.col(0).clone();
	Mat xVector = normalVectors.col(1).clone();
	Mat yVector = normalVectors.col(2).clone();

	Mat zMat = zVector.reshape(0, imageNumRow);
	xDifferentialMat = xVector.reshape(0, imageNumRow) / zMat;
	yDifferentialMat = yVector.reshape(0, imageNumRow) / zMat;
//	xDifferentialMat = xVector.reshape(0, imageNumRow);
//	yDifferentialMat = yVector.reshape(0, imageNumRow);
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
//	heightMat /= 20.0;  // scale
}

// private

void JacobiSolver::scaleDownDifferentialAndHeight(
		Mat xDifferential,
		Mat yDifferential,
		Mat heightMat,
		Mat& xDiffSmall,
		Mat& yDiffSmall,
		Mat& heightMatSmall) {

//	Size smallSize;
//	smallSize.height = heightMat.rows / 2;
//	smallSize.width = heightMat.cols / 2;

//	resize(xDifferential, xDiffSmall, smallSize);
//	resize(yDifferential, yDiffSmall, smallSize);
//	resize(heightMat, heightMatSmall, smallSize);

	pyrDown(xDifferential, xDiffSmall);
	pyrDown(yDifferential, yDiffSmall);
	yDiffSmall *= 2;
	xDiffSmall *= 2;
	pyrDown(heightMat, heightMatSmall);

//	Mat coarseXTail = xDiffSmall.colRange(1, smallSize.width);
//	coarseXTail += xDiffSmall.colRange(0, smallSize.width-1).clone();
//
//	Mat coarseYTail = yDiffSmall.rowRange(1, smallSize.height);
//	coarseYTail += yDiffSmall.rowRange(0, smallSize.height-1).clone();
}

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

//void JacobiSolver::computeDifferentialAdjust(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& adjust) {
//	int numRow = xDifferential.rows;
//	int numCol = xDifferential.cols;
////	adjust = Mat::zeros(numRow, numCol, CV_32FC1);
//
//	gpu::GpuMat yTail = yDifferential.rowRange(1, numRow);
//	gpu::GpuMat yHead = yDifferential.rowRange(0, numRow-1);
//	yTail -= yHead.clone();
//
//	gpu::GpuMat xTail = xDifferential.colRange(1, numCol);
//	gpu::GpuMat xHead = xDifferential.colRange(0, numCol-1);
//	xTail -= xHead.clone();
//
//	adjust += xDifferential;
//	adjust += yDifferential;
//	adjust /= 4.0;
//}


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

//void JacobiSolver::computeNeighborAdjust(gpu::GpuMat heightMat, gpu::GpuMat& adjust) {
//	int numRow = heightMat.rows;
//	int numCol = heightMat.cols;
//
//	adjust.setTo(0.0);
////	adjust = Mat::zeros(numRow, numCol, CV_32FC1);
//
//	gpu::GpuMat heightSubMat;
//	gpu::GpuMat adjustSubMat;
//
//	heightSubMat = heightMat.rowRange(1, numRow);
//	adjustSubMat = adjust.rowRange(0, numRow-1);
////	adjustSubMat += heightSubMat;
//	gpu::add(adjustSubMat, heightSubMat, adjustSubMat);
//
//	heightSubMat = heightMat.rowRange(0, numRow-1);
//	adjustSubMat = adjust.rowRange(1, numRow);
//	//	adjustSubMat += heightSubMat;
//	gpu::add(adjustSubMat, heightSubMat, adjustSubMat);
//
//	heightSubMat = heightMat.colRange(1, numCol);
//	adjustSubMat = adjust.colRange(0, numCol-1);
////	adjustSubMat += heightSubMat;
//	gpu::add(adjustSubMat, heightSubMat, adjustSubMat);
//
//	heightSubMat = heightMat.colRange(0, numCol-1);
//	adjustSubMat = adjust.colRange(1, numCol);
////	adjustSubMat += heightSubMat;
//	gpu::add(adjustSubMat, heightSubMat, adjustSubMat);
//
////	adjust /= 4.0;
//	gpu::divide(adjust, 4.0, adjust);
//}


void JacobiSolver::normalDifferentialToHeight(Mat xDifferential, Mat yDifferential, Mat& heightMat, unsigned int level) {

	int numRow = heightMat.rows;
	int numCol = heightMat.cols;

#ifdef TIME_CHECK
		boost::system_time last = boost::get_system_time();
#endif

	if (numRow > MIN_ROW_TO_SCALE && numCol > MIN_ROW_TO_SCALE) { // perform on scaled down version
		Mat coarseX;
		Mat coarseY;
		Mat coarseHeight;
		scaleDownDifferentialAndHeight(xDifferential, yDifferential, heightMat, coarseX, coarseY, coarseHeight);
		normalDifferentialToHeight(coarseX, coarseY, coarseHeight, ++level);
//		pyrUp(coarseHeight, heightMat);
//		resize(heightMat, heightMat, Size(numCol, numRow));
		resize(coarseHeight, heightMat, Size(numCol, numRow));
	}

//	numRow = heightMat.rows;
//	numCol = heightMat.cols;


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

//#ifdef TIME_CHECK
//		boost::system_time neighbor_start = boost::get_system_time();
//		boost::system_time add_start = boost::get_system_time();
//		boost::system_time neighbor_total = 0;
//		boost::system_time add_total = 0;
//#endif


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
//#ifdef TIME_CHECK
//		cout << "js loop 4: " << boost::get_system_time() - last << endl;
//		last = boost::get_system_time();
//#endif

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

