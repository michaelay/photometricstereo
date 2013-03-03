/*
 * LightingPanel.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#include <iostream>

#include "LightingPanelRenderer.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

using namespace cv;
using namespace std;

LightingPanelRenderer::LightingPanelRenderer() {
//	initDirectionMatList();
	mCurrentDirection = UP;
}

LightingPanelRenderer::~LightingPanelRenderer() {
}

void
LightingPanelRenderer::nextDirection() {
	// TODO: don't use case
	switch(mCurrentDirection) {
	case UP:
		mCurrentDirection = DOWN;
		break;
	case DOWN:
		mCurrentDirection = LEFT;
		break;
	case LEFT:
		mCurrentDirection = RIGHT;
		break;
	case RIGHT:
		mCurrentDirection = UP;
		break;
	default:
		mCurrentDirection = UP;
		break;
	}
}

void
LightingPanelRenderer::setDirection(LightingPanelDirection dir) {
	this->mCurrentDirection = dir;
}

void
LightingPanelRenderer::render() {
	GLint viewportCoordinates[4];
	glGetIntegerv(GL_VIEWPORT, viewportCoordinates); // x, y, width, height

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, viewportCoordinates[2], viewportCoordinates[3], 0, 0, 1); // left, right, bottom, top

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 1.0f, 1.0f);


	GLint x1, x2, y1, y2;
	// TODO: don't use case
	switch (this->mCurrentDirection) {
	case UP:
		x1 = 0;
		x2 = viewportCoordinates[2];
		y1 = 0;
		y2 = viewportCoordinates[3]/2;
		break;
	case DOWN:
		x1 = 0;
		x2 = viewportCoordinates[2];
		y1 = viewportCoordinates[3]/2;
		y2 = viewportCoordinates[3];
		break;
	case LEFT:
		x1 = 0;
		x2 = viewportCoordinates[2]/2;
		y1 = 0;
		y2 = viewportCoordinates[3];
		break;
	case RIGHT:
		x1 = viewportCoordinates[2]/2;
		x2 = viewportCoordinates[2];
		y1 = 0;
		y2 = viewportCoordinates[3];
		break;
	default:
		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = 0;
		break;
	}

	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();

	glMatrixMode (GL_PROJECTION);
	glPopMatrix();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix();
}
