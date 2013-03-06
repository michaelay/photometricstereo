/*
 * ImageGrabber.h
 *
 *  Created on: Aug 13, 2011
 *      Author: mistralay
 */

#ifndef IMAGEGRABBER_H_
#define IMAGEGRABBER_H_

#include "../constant.h"
#include "renderer/LightingPanelRenderer.h"

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
	vector<LightingPanelRenderer::LightingPanelDirection> mDirectionMap;
	static bool mHardcode;
	boost::system_time mLastUpdateScreen;

public:
	static void setHardcode(bool hardcode);
	static ImageGrabber* getInstance();

	virtual ~ImageGrabber();

	void updateScreenAndCapture(bool adjustInput=true);
	const Mat getImages();
	const Mat getImage(int idx);
	const Mat getImageColumn(int idx);

	const MatList getOriginalImages();
	void getAvgImage(Mat& avgImage);


private:
	ImageGrabber(); // create using getInstance only

	void updateScreen();
	void captureImage();
	void updateCurrentIdx();
	Mat convertToGrayScale(Mat image);
	string getFileName(int idx);
};

#endif /* IMAGEGRABBER_H_ */
