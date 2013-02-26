/*
 * GaussSeidelSolver.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#include "GaussSeidelSolver.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/gpu/gpu.hpp"
#include "constant.h"
#include <iostream>

GaussSeidelSolver::GaussSeidelSolver() {
	// TODO Auto-generated constructor stub

}

GaussSeidelSolver::~GaussSeidelSolver() {
	// TODO Auto-generated destructor stub
}

void GaussSeidelSolver::computeNormalDifferential(Mat normalVectors, int imageNumRow, Mat& xDifferentialMat, Mat& yDifferentialMat) {
	Mat zVector = normalVectors.col(0).clone();
	Mat xVector = normalVectors.col(1).clone();
	Mat yVector = normalVectors.col(2).clone();

	Mat zMat = zVector.reshape(0, imageNumRow);
	xDifferentialMat = xVector.reshape(0, imageNumRow) / zMat;
	yDifferentialMat = yVector.reshape(0, imageNumRow) / zMat;
}


void GaussSeidelSolver::normalToHeight(Mat normalVectors, int imageNumRow, Mat& heightMat) {
	Mat xDifferentialMat;
	Mat yDifferentialMat;
	computeNormalDifferential(normalVectors, imageNumRow, xDifferentialMat, yDifferentialMat);
	normalDifferentialToHeight(xDifferentialMat, yDifferentialMat, heightMat);
	heightMat /= 100.0;  // scale
}

//void GaussSeidelSolver::normalToHeightGpu(Mat normalVectors, int imageNumRow, Mat& heightMat) {
//	Mat xDifferentialMat;
//	Mat yDifferentialMat;
//	computeNormalDifferential(normalVectors, imageNumRow, xDifferentialMat, yDifferentialMat);
//
//	gpu::GpuMat heightMatGpu(heightMat);
//	normalDifferentialToHeightGpu(gpu::GpuMat(xDifferentialMat), gpu::GpuMat(yDifferentialMat), heightMatGpu);
//
//	heightMatGpu /= 100.0;
//	heightMatGpu.download(heightMat);
//}


// private

void GaussSeidelSolver::scaleDownDifferentialAndHeight(
		Mat xDifferential,
		Mat yDifferential,
		Mat heightMat,
		Mat& xDiffSmall,
		Mat& yDiffSmall,
		Mat& heightMatSmall) {

	Size smallSize;
	smallSize.height = heightMat.rows / 2;
	smallSize.width = heightMat.cols / 2;

	resize(xDifferential, xDiffSmall, smallSize);
	resize(yDifferential, yDiffSmall, smallSize);
	resize(heightMat, heightMatSmall, smallSize);

	Mat coarseXTail = xDiffSmall.colRange(1, smallSize.width);
	coarseXTail += xDiffSmall.colRange(0, smallSize.width-1).clone();

	Mat coarseYTail = yDiffSmall.rowRange(1, smallSize.height);
	coarseYTail += yDiffSmall.rowRange(0, smallSize.height-1).clone();
}

void GaussSeidelSolver::computeDifferentialAdjust(Mat xDifferential, Mat yDifferential, Mat& adjust) {
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

void GaussSeidelSolver::computeNeighborAdjust(Mat heightMat, Mat& adjust) {
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

void GaussSeidelSolver::normalDifferentialToHeight(Mat xDifferential, Mat yDifferential, Mat& heightMat) {

	int numRow = heightMat.rows;
	int numCol = heightMat.cols;

	if (numRow > MIN_ROW_TO_SCALE && numCol > MIN_ROW_TO_SCALE) { // perform on scaled down version
		std::cout << "pyramid" << std::endl;
		Mat coarseX;
		Mat coarseY;
		Mat coarseHeight;
		scaleDownDifferentialAndHeight(xDifferential, yDifferential, heightMat, coarseX, coarseY, coarseHeight);
		normalDifferentialToHeight(coarseX, coarseY, coarseHeight);
		resize(coarseHeight, heightMat, Size(numCol, numRow));
	}

	// run the actual gauss seidel algorithm
	Mat pqAdjust;
	computeDifferentialAdjust(xDifferential, yDifferential, pqAdjust);

	int k=0;
	for (k=0; k<MAX_NUM_ITERATION; k++) {
		bool hasChanged = false;

		Mat neightborAdjust;
		computeNeighborAdjust(heightMat, neightborAdjust);

		Mat oldHeight = heightMat.clone();

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


	}
	std::cout << "num iterations: " << k << std::endl;

}


//// gpu
//void GaussSeidelSolver::normalDifferentialToHeightGpu(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& heightMat) {
//
//	int numRow = heightMat.rows;
//	int numCol = heightMat.cols;
//
//	if (numRow > MIN_ROW_TO_SCALE && numCol > MIN_ROW_TO_SCALE) { // perform on scaled down version
//		gpu::GpuMat coarseX;
//		gpu::GpuMat coarseY;
//		gpu::GpuMat coarseHeight;
//		scaleDownDifferentialAndHeightGpu(xDifferential, yDifferential, heightMat, coarseX, coarseY, coarseHeight);
//		normalDifferentialToHeightGpu(coarseX, coarseY, coarseHeight);
//		gpu::resize(coarseHeight, heightMat, Size(numCol, numRow));
//	}
//
//	// run the actual gauss seidel algorithm
//	Mat pqAdjust;
//	computeDifferentialAdjust(xDifferential, yDifferential, pqAdjust);
//
//	int k=0;
//	for (k=0; k<MAX_NUM_ITERATION; k++) {
//		bool hasChanged = false;
//
//		gpu::GpuMat neightborAdjust;
//		computeNeighborAdjustGpu(heightMat, neightborAdjust);
//
//		gpu::GpuMat oldHeight = heightMat.clone();
//
//		heightMat = neightborAdjust + pqAdjust;
//
//		// convergence check
//		gpu::GpuMat absDiff;
//		gpu::absdiff(heightMat, oldHeight, absDiff);
//		Point maxPoint;
//		Point minPoint;
//		double min;
//		double max;
//		minMaxLoc(absDiff, &min, &max, &minPoint, &maxPoint);
//		if (max > GAUSS_SEIDEL_THRESHOLD) {
//			hasChanged = true;
//		}
//		if (!hasChanged) {
//			break;
//		}
//
//
//	}
//	std::cout << "num iterations: " << k << std::endl;
//
//}
//void GaussSeidelSolver::scaleDownDifferentialAndHeightGpu(
//		gpu::GpuMat xDifferential,
//		gpu::GpuMat yDifferential,
//		gpu::GpuMat heightMat,
//		gpu::GpuMat& xDiffSmall,
//		gpu::GpuMat& yDiffSmall,
//		gpu::GpuMat& heightMatSmall) {
//
//	Size smallSize;
//	smallSize.height = heightMat.rows / 2;
//	smallSize.width = heightMat.cols / 2;
//
//	resize(xDifferential, xDiffSmall, smallSize);
//	resize(yDifferential, yDiffSmall, smallSize);
//	resize(heightMat, heightMatSmall, smallSize);
//
//	gpu::GpuMat coarseXTail = xDiffSmall.colRange(1, smallSize.width);
//	coarseXTail += xDiffSmall.colRange(0, smallSize.width-1).clone();
//
//	gpu::GpuMat coarseYTail = yDiffSmall.rowRange(1, smallSize.height);
//	coarseYTail += yDiffSmall.rowRange(0, smallSize.height-1).clone();
//}
//
//void GaussSeidelSolver::computeDifferentialAdjustGpu(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& adjust) {
//	int numRow = xDifferential.rows;
//	int numCol = xDifferential.cols;
//	adjust = Mat::zeros(numRow, numCol, CV_32FC1);
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
//
//void GaussSeidelSolver::computeNeighborAdjustGpu(gpu::GpuMat heightMat, gpu::GpuMat& adjust) {
//	int numRow = heightMat.rows;
//	int numCol = heightMat.cols;
//
//	adjust = Mat::zeros(numRow, numCol, CV_32FC1);
//
//	gpu::GpuMat heightSubMat;
//	gpu::GpuMat adjustSubMat;
//
//	heightSubMat = heightMat.rowRange(1, numRow);
//	adjustSubMat = adjust.rowRange(0, numRow-1);
//	adjustSubMat += heightSubMat;
//
//	heightSubMat = heightMat.rowRange(0, numRow-1);
//	adjustSubMat = adjust.rowRange(1, numRow);
//	adjustSubMat += heightSubMat;
//
//	heightSubMat = heightMat.colRange(1, numCol);
//	adjustSubMat = adjust.colRange(0, numCol-1);
//	adjustSubMat += heightSubMat;
//
//	heightSubMat = heightMat.colRange(0, numCol-1);
//	adjustSubMat = adjust.colRange(1, numCol);
//	adjustSubMat += heightSubMat;
//
//	adjust /= 4.0;
//}

