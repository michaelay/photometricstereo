/*
 * ModelRenderer.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: mistralay
 */

#include "ModelRenderer.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "../constant.h"
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

// for mac
//#include <GLUT/glut.h>
// for windows freeglut
//#include <glut.h>
//#include <glext.h>


#define FOCAL_LENGTH 600
#define CUBE_SIZE 10

ModelRenderer* ModelRenderer::mInstance = 0;

//// static wrappers
//void ModelRenderer::displayWrapper() {
//	ModelRenderer::mInstance->display();
//}
//void ModelRenderer::idleWrapper() {
//	ModelRenderer::mInstance->idle();
//}
//void ModelRenderer::setInstance(ModelRenderer* instance) {
//	ModelRenderer::mInstance = instance;
//}
//void ModelRenderer::onOpenGL(void* userdata) {
////	ModelRenderer::mInstance->initialize();
//	ModelRenderer::mInstance->display();
//}


//// non static
//void ModelRenderer::initialize() {
//	glMatrixMode(GL_PROJECTION);												// select projection matrix
//	glViewport(0, 0, MODEL_WINDOW_WIDTH, MODEL_WINDOW_HEIGHT);									// set the viewport
//	glMatrixMode(GL_PROJECTION);												// set matrix mode
//	glLoadIdentity();															// reset projection matrix
//	GLfloat aspect = (GLfloat) MODEL_WINDOW_WIDTH / MODEL_WINDOW_HEIGHT;
//	gluPerspective(MODEL_WINDOW_FIELD_OF_VIEW_ANGLE, aspect, MODEL_WINDOW_Z_NEAR, MODEL_WINDOW_Z_FAR);		// set up a perspective projection matrix
//	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
//	glShadeModel( GL_SMOOTH );
//	glClearDepth( 1.0f );														// specify the clear value for the depth buffer
//	glEnable( GL_DEPTH_TEST );
//	glDepthFunc( GL_LEQUAL );
//	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );						// specify implementation-specific hints
//	glClearColor(0.0, 0.0, 0.0, 1.0);											// specify clear values for the color buffers
//}

void ModelRenderer::display() {

	GLuint mTexture;
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


	glDisable(GL_CULL_FACE);

	if (mHeightMap.rows != 0 && mHeightMap.cols != 0) {

//		float factor = 100.0f / mHeightMap.cols;
//		float factor = 1.0f / mHeightMap.cols;
//		glScalef(factor, factor, factor);
		glRotatef(mAngle, 0.0f, 1.0f, 0.0f);

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

//		glBegin(GL_TRIANGLE_STRIP);
		glBegin(GL_LINES);

		for (int r=numRow-1; r>0; r--) { // from numRow-1 to 1
			int currentRow = numRow-1-r;
			int upRow = currentRow+1;

//			for (int c=0; c<numCol; c++) {
//				glTexCoord2f((float)c / numCol, (float)(currentRow) / numRow);
//				glVertex3f(c - (numCol / 2), currentRow - (numRow/2), mHeightMap.at<float>(r, c)); // self
//
//				glTexCoord2f((float)c / numCol, (float)(upRow) / numRow);
//				glVertex3f(c - (numCol / 2), upRow - (numRow/2), mHeightMap.at<float>(r-1, c)); // above
//			}
//			glTexCoord2f((float)(numCol-1) / numCol, (float)(upRow) / numRow);
//			glVertex3f(numCol-1 - (numCol / 2), upRow - (numRow/2), mHeightMap.at<float>(r-1, numCol-1));
//
//			glTexCoord2f((float)0 / numCol, (float)(upRow) / numRow);
//			glVertex3f(0 - (numCol / 2), upRow - (numRow/2), mHeightMap.at<float>(r-1, 0)); // self


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
}
//void ModelRenderer::idle() {
//	//display();
//}

//void ModelRenderer::registerWindow() {
////	cv::createOpenGLCallback(WINDOW_OUTPUT, onOpenGL, NULL);
//}


ModelRenderer::ModelRenderer() {
	mAngle = MODEL_ANGLE_MIN;
	mSign = 1;
}

ModelRenderer::~ModelRenderer() {
	// TODO Auto-generated destructor stub
}

void ModelRenderer::update(Mat heightMap , Mat textureMap) {
	// resize height map
//	Mat smallHeightMap;
//	pyrDown(heightMap, smallHeightMap);
//	pyrDown(heightMap, smallHeightMap);
//	cvPyrDown(heightMap, smallHeightMap);
//	smallHeightMap /= 8;

	if (FLIP_HEIGHT) {
		mHeightMap = heightMap * -1 * HEIGHT_MAP_SCALE;
	} else {
		mHeightMap = heightMap * HEIGHT_MAP_SCALE;
	}
//
//	Size smallSize;
//	smallSize.height = 32;
//	smallSize.width = 24;
//	cvResize(mHeightMap, mHeightMap, smallSize);
//	mHeightMap /= 10;

	mTextureMap = textureMap;
	flip(mTextureMap, mTextureMap, 0);
	mTextureMap *= 2;

	if (MODEL_ROTATE) {
		// update angle
		mAngle += MODEL_ANGLE_STEP * mSign;
//		cout << "angle" << mAngle << endl;
		if (mAngle >= MODEL_ANGLE_MAX) {
//			cout << "plus" << endl;
			mSign = -1.0f;
		} else if (mAngle <= MODEL_ANGLE_MIN) {
//			cout << "neg" << endl;
			mSign = 1.0f;
		}
	}
}

