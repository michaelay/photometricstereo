/*
 * ModelRenderer.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: mistralay
 */

#include "ModelRenderer.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
using namespace cv;

#define FOCAL_LENGTH 600
#define CUBE_SIZE 10

ModelRenderer* ModelRenderer::mInstance = 0;

void ModelRenderer::render() {

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(70.0f, SCREEN_MARGIN_WIDTH / SCREEN_MARGIN_HEIGHT, 1, 200);

    // Select and setup the modelview matrix
    glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glColor3f(1.0f, 1.0f, 1.0f);

	if (mHeightMap.rows != 0 && mHeightMap.cols != 0) {

		glRotatef(mXAngle, 1.0f, 0.0f, 0.0f);
		glRotatef(mYAngle, 0.0f, 1.0f, 0.0f);

		int numRow = mHeightMap.rows;
		int numCol = mHeightMap.cols;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, numCol, numRow, 0, GL_BGR, GL_UNSIGNED_BYTE, mTextureMap.data);

		/*
		 * Triangle code starts here
		 * 3 verteces, 3 colors.
		 */
		glColor3f(0.5f, 0.5f, 0.5f);

		glTranslatef(-numCol/2, -numRow/2, 0);
		glTranslatef(0, -numCol/30, -100);

		glBegin(GL_LINES);

		for (int r=numRow-1; r>0; r--) { // from numRow-1 to 1
			int currentRow = numRow-1-r;
			int upRow = currentRow+1;

			for (int c=0; c<numCol; c++) {
				glTexCoord2f((float)c / numCol, (float)(currentRow) / numRow);
				glVertex3f(c, currentRow, mHeightMap.at<float>(r, c)); // self

				glTexCoord2f((float)c / numCol, (float)(upRow) / numRow);
				glVertex3f(c, upRow, mHeightMap.at<float>(r-1, c)); // above
			}
			glTexCoord2f((float)(numCol-1) / numCol, (float)(upRow) / numRow);
			glVertex3f(numCol-1, upRow, mHeightMap.at<float>(r-1, numCol-1));

			glTexCoord2f((float)0 / numCol, (float)(upRow) / numRow);
			glVertex3f(0, upRow, mHeightMap.at<float>(r-1, 0)); // self

		}

		glEnd();
	}

	glDisable(GL_TEXTURE_2D);


    glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix();

}

void
ModelRenderer::incrementAngle(bool x, bool y) {
	if (x) {
		updateAngle(mXAngle, true);
	}
	if (y) {
		updateAngle(mYAngle, true);
	}
}

void
ModelRenderer::decrementAngle(bool x, bool y) {
	if (x) {
		updateAngle(mXAngle, false);
	}
	if (y) {
		updateAngle(mYAngle, false);
	}
}

ModelRenderer::ModelRenderer() {
	mXAngle = ANGLE_DEFAULT;
	mYAngle = ANGLE_DEFAULT;
}

ModelRenderer::~ModelRenderer() {
	// TODO Auto-generated destructor stub
}

void
ModelRenderer::setHeightMap(cv::Mat heightMap) {
	mHeightMap = heightMap;
}

void
ModelRenderer::setTextureMap(cv::Mat textureMap) {
	mTextureMap = textureMap;
}

// private helper
void
ModelRenderer::updateAngle(float& angle, bool increment) {
	if (increment) {
		if (angle + ANGLE_STEP_SIZE > ANGLE_MAX) {
			angle = ANGLE_MAX;
		} else {
			angle += ANGLE_STEP_SIZE;
		}
	} else {
		if (angle - ANGLE_STEP_SIZE < ANGLE_MIN) {
			angle = ANGLE_MIN;
		} else {
			angle -= ANGLE_STEP_SIZE;
		}
	}
}
