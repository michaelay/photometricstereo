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

void
JacobiSolver::normalToHeightIterative(Mat normals, int numRow, Mat height) {
	boost::system_time last = boost::get_system_time();
	// init
	int numCol = normals.rows / numRow;

	// compute nx and ny
//	Mat nx = (normals.col(1).reshape(0, numRow) / normals.col(0).reshape(0, numRow));
//	Mat ny = (normals.col(2).reshape(0, numRow) / normals.col(0).reshape(0, numRow));
	Mat nx, ny;
	divide(normals.col(1), normals.col(0), nx);
	nx = nx.reshape(0, numRow);
	divide(normals.col(2), normals.col(0), ny);
	ny = ny.reshape(0, numRow);
	cout << "differential: " << boost::get_system_time() - last << endl; last = boost::get_system_time();

	// calculate number of iterations needed, take log2 of max(numRow, numCol)
	unsigned int numIteration = 0;
	int tmpIntForLog2 = max(numRow, numCol);
	while (tmpIntForLog2 >>= 1) {
		++numIteration;
	}
	// minimum 2^4 pixels
	numIteration -= 3;

	if (mHeightPyramid.size() == 0
		|| mHeightPyramid.size() != numIteration
		|| mHeightPyramid.at(0).rows != numRow
		|| mHeightPyramid.at(0).cols != numCol) {
		// create pyramids if not already
		cout << "Create pyramid" << endl;
		mHeightPyramid.resize(numIteration);
		mHeightPyramid2.resize(numIteration);
		mNxPyramid.resize(numIteration);
		mNyPyramid.resize(numIteration);

		mHeightPyramid[0] = height;
		mHeightPyramid2[0] = height.clone();
	}
	mNxPyramid[0] = nx;
	mNyPyramid[0] = ny;
	cout << "setup pyramid: " << boost::get_system_time() - last << endl; last = boost::get_system_time();

	// fill the pyramid
	for (unsigned int i = 1; i < numIteration; i++) {
		pyrDown(mHeightPyramid[i-1], mHeightPyramid[i], Size(mHeightPyramid[i-1].cols/2, mHeightPyramid[i-1].rows/2)); // TODO: only need last level
		mHeightPyramid2[i] = mHeightPyramid[i].clone(); // TODO: no need to clone, just init
		pyrDown(mNxPyramid[i-1], mNxPyramid[i], Size(mNxPyramid[i-1].cols/2, mNxPyramid[i-1].rows/2));
		pyrDown(mNyPyramid[i-1], mNyPyramid[i], Size(mNyPyramid[i-1].cols/2, mNyPyramid[i-1].rows/2));
	}
	cout << "filled pyramid: " << boost::get_system_time() - last << endl; last = boost::get_system_time();

	// loop through numIteration, (numIteration - 1) to 0, i is iteration number
	for (int i = numIteration-1; i >= 0; i--) {
		double differentialMultiple = pow(2.0,i);

		numRow = mHeightPyramid[i].rows;
		numCol = mHeightPyramid[i].cols;

		// ignore the four edges
		bool usePyramid2 = true; // use pyramid 2 as target ( update pyramid 2 )
		int numLeft = MAX_NUM_ITERATION;
		Mat srcHeight, targetHeight; // = mHeightPyramid[i]; //, targetHeight;
//		Mat newValue;
		double lastMaxAbsVal = 0;
		double lastMinAbsVal = 0;

		while (numLeft-- > 0) {

			if (usePyramid2) {
				srcHeight = mHeightPyramid[i];
				targetHeight = mHeightPyramid2[i];
			} else {
				srcHeight = mHeightPyramid2[i];
				targetHeight = mHeightPyramid[i];
			}
//			srcHeight = mHeightPyramid[i];
//			targetHeight = mHeightPyramid[i];

			// center piece
			targetHeight(Range(1, numRow-1), Range(1, numCol-1))
				= (
					srcHeight(Range(2, numRow), Range(1, numCol-1)) // height_(y+1, x)
					+ srcHeight(Range(0, numRow-2), Range(1, numCol-1)) // height_(y-1, x)
					+ srcHeight(Range(1, numRow-1), Range(2, numCol)) // height_(y, x+1)
					+ srcHeight(Range(1, numRow-1), Range(0, numCol-2)) // height_(y, x-1)
					+ (
						mNxPyramid[i](Range(1, numRow-1), Range(1, numCol-1)) // nx_(y, x)
						- mNxPyramid[i](Range(1, numRow-1), Range(0, numCol-2)) // nx_(y, x-1)
						+ mNyPyramid[i](Range(1, numRow-1), Range(1, numCol-1)) // ny_(y, x)
						- mNyPyramid[i](Range(0, numRow-2), Range(1, numCol-1)) // ny_(y-1, x)
					) * differentialMultiple
				) / 4.0 + 0.0;
//			targetHeight(Range(1, numRow-1), Range(1, numCol-1)) = newValue;
//			// top edge
//			mHeightPyramid[i](Range(0, 1), Range(1, numCol-1))
//				= (
//					mHeightPyramid[i](Range(0, 1), Range(2, numCol)) // height_(y, x+1)
//					+ mHeightPyramid[i](Range(0, 1), Range(0, numCol-2)) // height_(y, x-1)
//					+ mHeightPyramid[i](Range(1, 2), Range(1, numCol-1)) // height_(y+1, x)
//					+ mHeightPyramid[i](Range(0, 1), Range(1, numCol-1)) // height_(y, x) <-- self
//					+ (
//						mNxPyramid[i](Range(0, 1), Range(1, numCol-1)) // nx_(y, x) // assume 0 Ny
//						- mNxPyramid[i](Range(0, 1), Range(0, numCol-2)) // nx_(y, x-1)
//					) * differentialMultiple
//				) / 4.0;
//			// bottom edge
//			mHeightPyramid[i](Range(numRow-1, numRow), Range(1, numCol-1))
//				= (
//					mHeightPyramid[i](Range(numRow-1, numRow), Range(2, numCol)) // height_(y, x+1)
//					+ mHeightPyramid[i](Range(numRow-1, numRow), Range(0, numCol-2)) // height_(y, x-1)
//					+ mHeightPyramid[i](Range(numRow-2, numRow-1), Range(1, numCol-1)) // height_(y-1, x)
//					+ mHeightPyramid[i](Range(numRow-1, numRow), Range(1, numCol-1)) // height_(y, x) <-- self
//					+ (
//						mNxPyramid[i](Range(numRow-1, numRow), Range(1, numCol-1)) // nx_(y, x)
//						- mNxPyramid[i](Range(numRow-1, numRow), Range(0, numCol-2)) // nx_(y, x-1)
//						+ mNyPyramid[i](Range(numRow-1, numRow), Range(1, numCol-1)) // ny_(y, x)
//						- mNyPyramid[i](Range(numRow-2, numRow-1), Range(1, numCol-1)) // ny_(y-1, x)
//					) * differentialMultiple
//				) / 4.0;
//			// left edge
//			mHeightPyramid[i](Range(1, numRow-1), Range(0, 1))
//				= (
//					mHeightPyramid[i](Range(0, numRow-2), Range(0, 1)) // height_(y-1, x)
//					+ mHeightPyramid[i](Range(2, numRow), Range(0, 1)) // height_(y+1, x)
//					+ mHeightPyramid[i](Range(1, numRow-1), Range(1, 2)) // height_(y, x+1)
//					+ mHeightPyramid[i](Range(1, numRow-1), Range(0, 1)) // height_(y, x)
//					+ (
//						mNyPyramid[i](Range(1, numRow-1), Range(0, 1)) // ny_(y, x) // assume 0 Nx
//						- mNyPyramid[i](Range(0, numRow-2), Range(0, 1)) // ny_(y-1, x)
//					) * differentialMultiple
//				) / 4.0;
//			// right edge
//			mHeightPyramid[i](Range(1, numRow-1), Range(numCol-1, numCol))
//				= (
//					mHeightPyramid[i](Range(0, numRow-2), Range(numCol-1, numCol)) // height_(y-1, x)
//					+ mHeightPyramid[i](Range(2, numRow), Range(numCol-1, numCol)) // height_(y+1, x)
//					+ mHeightPyramid[i](Range(1, numRow-1), Range(numCol-2, numCol-1)) // height_(y, x-1)
//					+ mHeightPyramid[i](Range(1, numRow-1), Range(numCol-1, numCol)) // height_(y, x)
//					+ (
//						mNxPyramid[i](Range(1, numRow-1), Range(numCol-1, numCol)) // nx_(y, x)
//						- mNxPyramid[i](Range(1, numRow-1), Range(numCol-2, numCol-1)) // nx_(y, x-1)
//						+ mNyPyramid[i](Range(1, numRow-1), Range(numCol-1, numCol)) // ny_(y, x)
//						- mNyPyramid[i](Range(0, numRow-2), Range(numCol-1, numCol)) // ny_(y-1, x)
//					) * differentialMultiple
//				) / 4.0;
//			// corners
//			// top left
//			mHeightPyramid[i].at<float>(0, 0)
//					= (
//					    mHeightPyramid[i].at<float>(0, 0) * 2 // y, x
//					    + mHeightPyramid[i].at<float>(0, 1) // y, x+1
//					    + mHeightPyramid[i].at<float>(1, 0) // y+1, x
//					) / 4.0;
//			// bottom left
//			mHeightPyramid[i].at<float>(numRow-1, 0)
//					= (
//					    mHeightPyramid[i].at<float>(numRow-1, 0) * 2 // y, x
//					    + mHeightPyramid[i].at<float>(numRow-1, 1) // y, x+1
//					    + mHeightPyramid[i].at<float>(numRow-2, 0) // y-1, x
//					    + (mNyPyramid[i].at<float>(numRow-1, 0) // ny_(y, x)
//					       - mNyPyramid[i].at<float>(numRow-2, 0) // ny_(y-1, x)
//					       ) * differentialMultiple
//					) / 4.0;
//			// top right
//			mHeightPyramid[i].at<float>(0, numCol-1)
//					= (
//					    mHeightPyramid[i].at<float>(0, numCol-1) * 2 // y, x
//					    + mHeightPyramid[i].at<float>(0, numCol-2) // y, x-1
//					    + mHeightPyramid[i].at<float>(1, numCol-1) // y+1, x
//					    + (mNxPyramid[i].at<float>(0, numCol-1) // nx_(y, x)
//					       - mNxPyramid[i].at<float>(0, numCol-2) // nx_(y, x-1)
//					       ) * differentialMultiple
//					) / 4.0;
//			// bottom right
//			mHeightPyramid[i].at<float>(numRow-1, numCol-1)
//					= (
//					    mHeightPyramid[i].at<float>(numRow-1, numCol-1) * 2 // y, x
//					    + mHeightPyramid[i].at<float>(numRow-1, numCol-2) // y, x-1
//					    + mHeightPyramid[i].at<float>(numRow-2, numCol-1) // y-1, x
//					    + (mNxPyramid[i].at<float>(numRow-1, numCol-1) // nx_(y, x)
//					       - mNxPyramid[i].at<float>(numRow-1, numCol-2) // nx_(y, x-1)
//						   + mNyPyramid[i].at<float>(numRow-1, numCol-1) // ny_(y, x)
//						   - mNyPyramid[i].at<float>(numRow-2, numCol-1) // ny_(y-1, x)
//					       ) * differentialMultiple
//					) / 4.0;

			// check convergence
			if (numLeft < MAX_NUM_ITERATION) { // do not exit on first 2 iteration
				double minVal, maxVal, maxAbsVal, minAbsVal;
				minMaxLoc(targetHeight - srcHeight, &minVal, &maxVal); //find minimum and maximum intensities
//				minMaxLoc(newValue - srcHeight(Range(1, numRow-1), Range(1, numCol-1)), &minVal, &maxVal); //find minimum and maximum intensities
//				cout << "max: " << abs(maxVal) << " min: " << abs(minVal) << endl;
				maxAbsVal = max(abs(minVal), abs(maxVal));
				minAbsVal = min(abs(minVal), abs(maxVal));
//				if ((lastMaxAbsVal == maxAbsVal && lastMinAbsVal == minAbsVal)
//						|| maxAbsVal <= GAUSS_SEIDEL_THRESHOLD) {
				if ((abs(lastMaxAbsVal - maxAbsVal) <= GAUSS_SEIDEL_THRESHOLD * differentialMultiple
						&& abs(lastMinAbsVal - minAbsVal) <= GAUSS_SEIDEL_THRESHOLD * differentialMultiple)
						|| maxAbsVal <= GAUSS_SEIDEL_THRESHOLD * differentialMultiple) {
//				if (checkVal <= GAUSS_SEIDEL_THRESHOLD) {
					if (usePyramid2) { // if pyramid2 is most updated, copy the value to pyramid1
//						targetHeight.copyTo(srcHeight);
						srcHeight = targetHeight;
					}
					cout << "num iteration: " << MAX_NUM_ITERATION - numLeft << endl;
					break;
				}
				lastMaxAbsVal = maxAbsVal;
				lastMinAbsVal = minAbsVal;
			}

			usePyramid2 = !usePyramid2;
//			srcHeight(Range(1, numRow-1), Range(1, numCol-1)) = newValue;
		}

		if (i > 0) {
			pyrUp(mHeightPyramid[i], mHeightPyramid[i-1], Size(mNxPyramid[i-1].cols, mNxPyramid[i-1].rows));
		}
		cout << "done iteration " << i << ": " << boost::get_system_time() - last << endl; last = boost::get_system_time();

	}
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

