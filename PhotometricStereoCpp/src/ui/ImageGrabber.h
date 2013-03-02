/*
 * ImageGrabber.h
 *
 *  Created on: Aug 13, 2011
 *      Author: mistralay
 */

#ifndef IMAGEGRABBER_H_
#define IMAGEGRABBER_H_

#include "../constant.h"
#include "LightingPanel.h"
#include "InputAdjuster.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "boost/thread.hpp"
#include "boost/date_time.hpp"

using namespace std;
using namespace cv;

typedef vector<Mat > MatList;

class ImageGrabber {
private:
	Mat mImages; // p x n matrix, each column is an image
	Mat mOriginalImages;
	int mCurrentIdx;
	Ptr<VideoCapture> mCapture;
	vector<LightingPanel::LightingPanelDirection> mDirectionMap;
//	LightingPanel mLightPanel;
	bool mHardcode;
	bool mSaveCopy;
	boost::system_time mLastUpdateScreen;
	InputAdjuster mAdjuster;

public:
	ImageGrabber(bool hardcode=false, bool saveCopy=false);
	virtual ~ImageGrabber();

	void updateScreenAndCapture(bool adjustInput=true);
	const Mat getImages();
	const Mat getImage(int idx);
	const Mat getImageColumn(int idx);

	const MatList getOriginalImages();
	void getAvgImage(Mat& avgImage);


private:
	void updateScreen();
	void captureImage();
	void captureImage(bool adjustInput);
	void updateCurrentIdx();
	Mat convertToGrayScale(Mat image);
	string getFileName(int idx);
};

#endif /* IMAGEGRABBER_H_ */
