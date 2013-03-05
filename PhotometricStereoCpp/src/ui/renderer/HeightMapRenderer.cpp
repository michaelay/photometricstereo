/*
 * HeightMapRenderer.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: michaelay
 */

#include "HeightMapRenderer.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <opencv2/highgui/highgui.hpp>

HeightMapRenderer::HeightMapRenderer() {
	// TODO Auto-generated constructor stub

}

HeightMapRenderer::~HeightMapRenderer() {
	// TODO Auto-generated destructor stub
}

void
HeightMapRenderer::setHeightMap(cv::Mat heightMap) {
	mHeightMap = heightMap;
}

void
HeightMapRenderer::render() {
//	GLint viewportCoordinates[4];
//	glGetIntegerv(GL_VIEWPORT, viewportCoordinates); // x, y, width, height
//
//	glMatrixMode (GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	glOrtho(0, viewportCoordinates[2], viewportCoordinates[3], 0, 0, 1); // left, right, bottom, top
//
//	glMatrixMode (GL_MODELVIEW);
//	glPushMatrix();
//
//	glEnable(GL_DEPTH_TEST);
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
////	glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glColor3f(1.0f, 1.0f, 1.0f);
//
////	glEnable(GL_DEPTH_TEST);
////	glRasterPos2f(-1.0f , -0.4f);
//	glRasterPos2i(viewportCoordinates[0], viewportCoordinates[1]);
//	glDrawPixels(mHeightMap.cols, mHeightMap.rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, mHeightMap.data);
////	glDrawPixels(viewportCoordinates[2], viewportCoordinates[3], GL_LUMINANCE, GL_UNSIGNED_BYTE, mHeightMap.data);
////	glRasterPos2f(-1.0f, -1.0f);
//
//	glPopMatrix();
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();

	double minVal, maxVal;
	minMaxLoc(mHeightMap, &minVal, &maxVal); //find minimum and maximum intensities

	cv::namedWindow("height map", CV_WINDOW_AUTOSIZE);
	cv::imshow("height map", (mHeightMap - cv::Scalar::all(-1.0 * minVal)) * (1.0 / (maxVal - minVal)));
}


