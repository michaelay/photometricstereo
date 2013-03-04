/*
 * PhotometricStereo.h
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#ifndef PHOTOMETRICSTEREO_H_
#define PHOTOMETRICSTEREO_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "algo/JacobiSolver.h"

using namespace cv;

class PhotometricStereo {
private:
	Mat mExpectedL;
	float mExpectedLvalues[4][2];
	float mExpectedLz;
	JacobiSolver solver;

	Mat mNormalMap;
	Mat mHeightMap;

public:
	PhotometricStereo();
	virtual ~PhotometricStereo();

	void solve(Mat images);
	Mat getNormalMap();
	Mat getHeightMap();

private:
//	void getROI(Mat images, Mat& roi);
	void getNormal(Mat images);
	void getHeight();
//	void getTextureMap(Mat images, Mat textureMap);


//	void showROI(const string& windowName);
	void showNormal(const string& windowName, Mat normals);
	void showHeightMap(const string& windowName, Mat heightMat);

};

#endif /* PHOTOMETRICSTEREO_H_ */
