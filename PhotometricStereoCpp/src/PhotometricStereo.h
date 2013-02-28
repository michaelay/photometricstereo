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
#include "JacobiSolver.h"

using namespace cv;

class PhotometricStereo {
private:
	Mat mExpectedL;
	float mExpectedLvalues[4][2];
	float mExpectedLz;
	JacobiSolver solver;
//	VideoWriter mVideoWriter;
public:
	PhotometricStereo();
	virtual ~PhotometricStereo();

	void getHeightMap(Mat images, Mat heightMap);

private:
	void getROI(Mat images, Mat& roi);
	Mat getNormal(Mat images, Mat roi);
	void getHeight(Mat normals, Mat roi, Mat heightMap);
//	void getTextureMap(Mat images, Mat textureMap);


	void showROI(const string& windowName, Mat roi);
	void showNormal(const string& windowName, Mat normals);
	void showHeightMap(const string& windowName, Mat heightMat);

};

#endif /* PHOTOMETRICSTEREO_H_ */
