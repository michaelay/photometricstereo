/*
 * InputAdjuster.cpp
 *
 *  Created on: 2011¦~8¤ë18¤é
 *      Author: mistralay
 */


#include "InputAdjuster.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "constant.h"

InputAdjuster::InputAdjuster() {
	// TODO Auto-generated constructor stub
}

InputAdjuster::~InputAdjuster() {
	// TODO Auto-generated destructor stub
}

void InputAdjuster::createAdjustment(Mat images, Mat& adjustment) {
	calculateAdjustmentMat(images, adjustment);

	FileStorage fs(ADJUSTMENT_MAT_FILENAME, FileStorage::WRITE);
	fs << ADJUSTMENT_MAT_KEY << adjustment;
}

void InputAdjuster::getAdjustment(Mat& adjustment) {
	if (mAdjustmentMat.cols == 0 && mAdjustmentMat.rows == 0) {
		FileStorage fs(ADJUSTMENT_MAT_FILENAME, FileStorage::READ);
		fs[ADJUSTMENT_MAT_KEY] >> mAdjustmentMat;
//		mAdjustmnentMat = Mat(cvLoad(ADJUSTMENT_MAT_FILENAME));
	}
	adjustment = mAdjustmentMat;
}

void InputAdjuster::adjustInput(Mat& images) {
	Mat adjMat;
	getAdjustment(adjMat);
	multiply(images, adjMat, images);
}

void InputAdjuster::adjustInput(Mat& images, Mat roi) {
	adjustInput(images);
//	Mat col;
//	for (int i=0; i<NUM_IMAGES; i++) {
//		col = images.col(i);
//		col.setTo(0, ~roi);
//	}
}

void InputAdjuster::adjustInputCol(Mat& imageCol, int idx) {
	Mat adjMat;
	getAdjustment(adjMat);
	Mat adjCol;
	adjCol = adjMat.col(idx); // * 3;
	multiply(imageCol, adjCol, imageCol);
}

void InputAdjuster::adjustInputCol(Mat& imageCol, Mat roi, int idx) {
	adjustInputCol(imageCol, idx);
}

// private
void InputAdjuster::calculateAdjustmentMat(Mat images, Mat& adjustmentMat) {
	// for each column of images (each image), reshape, smooth, reshape
	adjustmentMat = Mat::zeros(CAPTURE_HEIGHT*CAPTURE_WIDTH, NUM_IMAGES, CV_32FC1);
	Mat column;
	Mat image;
	for (int i=0; i<NUM_IMAGES; i++) {
		column = images.col(i).clone();
		image = column.reshape(0, CAPTURE_HEIGHT);
		blur(image, image, Size(7,7));

		column = image.clone().reshape(0, CAPTURE_HEIGHT*CAPTURE_WIDTH);
//		double minValue = 0;
//		double maxValue = 0;
//		minMaxLoc(column, &minValue, &maxValue);
		column = 1.0 / column;

		Mat adjCol;
		adjCol = adjustmentMat.col(i);
		column.copyTo(adjCol);
	}
}
void InputAdjuster::calculateAdjustmentImage(Mat image, Mat adjustmentImage) {

}


