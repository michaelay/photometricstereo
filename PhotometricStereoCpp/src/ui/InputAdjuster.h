/*
 * InputAdjuster.h
 *
 *  Created on: 2011¦~8¤ë18¤é
 *      Author: mistralay
 */

#ifndef INPUTADJUSTER_H_
#define INPUTADJUSTER_H_

#include <opencv2/core/core.hpp>

using namespace cv;

class InputAdjuster {
private:
	Mat mAdjustmentMat;

public:
	InputAdjuster();
	virtual ~InputAdjuster();

	void createAdjustment(Mat images, Mat& adjustment);
	void adjustInput(Mat& images, Mat roi);
	void adjustInput(Mat& images);
	void getAdjustment(Mat& adjustment);
	void adjustInputCol(Mat& imageCol, int idx);
	void adjustInputCol(Mat& imageCol, Mat roi, int idx);

private:
	void calculateAdjustmentMat(Mat images, Mat& adjustmentMat);
	void calculateAdjustmentImage(Mat image, Mat adjustmentImage);

};

#endif /* INPUTADJUSTER_H_ */
