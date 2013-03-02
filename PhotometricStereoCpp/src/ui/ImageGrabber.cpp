/*
 * ImageGrabber.cpp
 *
 *  Created on: Aug 13, 2011
 *      Author: mistralay
 */

#include "ImageGrabber.h"
#include "InputAdjuster.h"
#include <iostream>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "boost/thread.hpp"
#include "Display.h"

using namespace std;

ImageGrabber::ImageGrabber(bool hardcode /*=false*/, bool saveCopy /*=false*/) {
	mHardcode = hardcode;
	mSaveCopy = saveCopy;

	if (!mHardcode) {
		mCapture = new VideoCapture(WEBCAM_NUMBER);
		if (!mCapture->isOpened()) {
			cerr << "Failed to open webcam" << endl;
			Exception exception;
			exception.msg = "Failed to open webcam";
			throw exception;
		}

		mCapture->set(CV_CAP_PROP_FRAME_WIDTH, CAPTURE_WIDTH);
		mCapture->set(CV_CAP_PROP_FRAME_HEIGHT, CAPTURE_HEIGHT);
		mCapture->set(CV_CAP_PROP_FPS, CAPTURE_FPS);
		//	mCapture.set(CV_CAP_PROP_CONVERT_RGB, 1);
	}

	// init mDirectionMap
	mDirectionMap.resize(LightingPanel::NUM_DIRECTION);
	mDirectionMap.at(0) = LightingPanel::UP;
	mDirectionMap.at(1) = LightingPanel::DOWN;
	mDirectionMap.at(2) = LightingPanel::LEFT;
	mDirectionMap.at(3) = LightingPanel::RIGHT;

	mOriginalImages.resize(NUM_IMAGES);

	mImages.create(CAPTURE_HEIGHT * CAPTURE_WIDTH, NUM_IMAGES, CV_32FC1);
	mOriginalImages.create(CAPTURE_HEIGHT, CAPTURE_WIDTH, CV_8UC3);

	mLastUpdateScreen = boost::get_system_time();
	mCurrentIdx = 0;
}

ImageGrabber::~ImageGrabber() {
	if (!mCapture.empty() && mCapture->isOpened()) {
		mCapture->release();
	}
}

/**
 * Convert captured image from BGR to RGB, and scale correctly.
 * Creates new matrix like CvtColor
 */
void ImageGrabber::updateScreenAndCapture(bool adjustInput) {
//	updateScreen();
//
//	boost::system_time captureTime = mLastUpdateScreen + boost::posix_time::millisec(CAPTURE_WAIT_MILLISECOND);
//	boost::thread::sleep(captureTime);
//
//	captureImage();
//	updateCurrentIdx();
//	updateScreen();
//	mLastUpdateScreen = boost::get_system_time();

	// NEW
	boost::system_time captureTime = mLastUpdateScreen + boost::posix_time::millisec(CAPTURE_WAIT_MILLISECOND);
	boost::thread::sleep(captureTime);
	captureImage();

	updateCurrentIdx();
	updateScreen();
	mLastUpdateScreen = boost::get_system_time();
}

const Mat ImageGrabber::getImages() {
	return mImages;
}

/**
 * Returns a clone
 */
const Mat ImageGrabber::getImage(int idx) {
	Mat column = getImageColumn(idx);
	Mat image = column.clone().reshape(0, CAPTURE_HEIGHT);
	return image;
}

const Mat ImageGrabber::getImageColumn(int idx) {
	// input validation
	if (idx < 0 || idx >= NUM_IMAGES) {
		throw new Exception();
	}
	return getImages().col(idx);
}

const MatList ImageGrabber::getOriginalImages() {
	return mOriginalImages;
}

void ImageGrabber::getAvgImage(Mat& avgImage) {
	avgImage = mOriginalImages;
}


// private methos
void ImageGrabber::updateScreen() {
//	mLightPanel.showDirection(mDirectionMap.at(mCurrentIdx));
	Display* display = Display::getInstance();
	display->updateLight((Display::LightingDirection)mDirectionMap.at(mCurrentIdx));
	display->draw();
}

void ImageGrabber::captureImage() {
	captureImage(true);
}

void ImageGrabber::captureImage(bool adjustInput) {

	int destIdx;
	Mat newImage;
	if (mHardcode) {
		destIdx = mCurrentIdx;
		std::cout << "file to read: " << getFileName(destIdx) << std::endl;
		newImage = imread(getFileName(destIdx));
	} else {
		if (CAPTURE_IDX_OFFSET != 0) {
			destIdx = (mCurrentIdx + CAPTURE_IDX_OFFSET) % NUM_IMAGES;
			if (destIdx < 0) {
				destIdx += NUM_IMAGES;
			}
		} else {
			destIdx = mCurrentIdx;
		}

		// grab and retrieve the image
//		mCapture->grab();
//		mCapture->retrieve(newImage);

		// NEW
		mCapture->grab();
		*mCapture >> newImage;

		if (mSaveCopy) {
			imwrite(getFileName(destIdx), newImage);
		}
	}

	if (destIdx == 0) {
		mOriginalImages = newImage.clone();
	}

	// convert image to grayscale column vector
	Mat newImageConvert = convertToGrayScale(newImage);
	Mat newImageColMat = newImageConvert.reshape(0, CAPTURE_HEIGHT*CAPTURE_WIDTH);

//	if (adjustInput) {
//		mAdjuster.adjustInputCol(newImageColMat, destIdx);
//	}

	// copy to corresponding column of mImages
	Mat destinationCol = mImages.col(destIdx);
	newImageColMat.copyTo(destinationCol);

	// NEW
//	if (!mHardcode) {
//		// clear the cache
//		// capture image from webcam
//		Mat dummy;
//		for (int i=0; i<WEBCAM_NUMBER_FLUSH; i++) {
//			mCapture->retrieve(dummy);
//		}
//	}
}

void ImageGrabber::updateCurrentIdx() {
	mCurrentIdx = ++mCurrentIdx % NUM_IMAGES;
}

Mat ImageGrabber::convertToGrayScale(Mat image) {
	Mat converted;
	cvtColor(image, image, CV_BGR2GRAY); // 8U, 0-256
	image.convertTo(converted, CV_32F); // 32F, 0-256, cannot be displayed
	converted /= 256; // 32F, 0.0 - 1.0
	return converted;
}

string ImageGrabber::getFileName(int idx) {
	stringstream ss;
	ss << SAVE_FILENAME_PREFIX << idx << ".png";
	return ss.str();
}
