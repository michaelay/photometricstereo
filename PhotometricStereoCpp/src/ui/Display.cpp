/*
 * Display.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: mistralay
 */

#include "Display.h"
#include <GL/glfw.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../constant.h"
#include <iostream>

using namespace cv;
using namespace std;

Display* Display::mInstance = NULL;
Display* Display::getInstance() {
	if (mInstance == NULL) {
		mInstance = new Display();
	}
	return mInstance;
}

Display::Display() {
	mDir = UP;
    if( !glfwInit() )
    {
        cerr << "Failed to initialize GLFW" << endl;
    }

	if( !glfwOpenWindow( SCREEN_WIDTH, SCREEN_HEIGHT, 0,0,0,0,0,0, GLFW_WINDOW ) )
	{
		glfwTerminate();
		cerr << "Failed to open windows in GLFW" << endl;
	}

	// init opengl
	glMatrixMode(GL_MODELVIEW);
	glEnable( GL_SCISSOR_TEST );
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Display::~Display() {
	glfwTerminate();
}

void Display::draw() {
	drawLight();
	drawModel();
	drawHeightMap();

	// Swap buffers
    glfwSwapBuffers();
}

void Display::waitKey() {
	cout << "wait key" << endl;
	while (glfwGetWindowParam( GLFW_OPENED )
//			&& !glfwGetKey(GLFW_KEY_ESC)
//			&& !glfwGetKey(GLFW_KEY_SPACE)
			&& !glfwGetKey(GLFW_KEY_ENTER)) {
//		usleep(1000*100); // 100 millisecond
		sleep(1);
		cout << "wait key inner" << endl;
	}
}

void Display::drawLight() {

	glViewport(SCREEN_MARGIN_WIDTH, 0, SCREEN_HEIGHT, SCREEN_HEIGHT);
//	glScissor(SCREEN_MARGIN_WIDTH, 0, SCREEN_HEIGHT, SCREEN_HEIGHT);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, SCREEN_HEIGHT, SCREEN_HEIGHT, 0, 0, 1);

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 1.0f, 1.0f);

	float x1 = 0;
	float x2 = 0;
	float y1 = 0;
	float y2 = 0;

	switch (mDir) {
	case UP:
		x1 = 0;
		x2 = SCREEN_HEIGHT;
		y1 = 0;
		y2 = SCREEN_HEIGHT/2;
		break;
	case DOWN:
		x1 = 0;
		x2 = SCREEN_HEIGHT;
		y1 = SCREEN_HEIGHT/2;
		y2 = SCREEN_HEIGHT;
		break;
	case LEFT:
		x1 = 0;
		x2 = SCREEN_HEIGHT/2;
		y1 = 0;
		y2 = SCREEN_HEIGHT;
		break;
	case RIGHT:
		x1 = SCREEN_HEIGHT/2;
		x2 = SCREEN_HEIGHT;
		y1 = 0;
		y2 = SCREEN_HEIGHT;
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

	glPopMatrix();

	glMatrixMode (GL_PROJECTION);
	glPopMatrix();

    glMatrixMode( GL_MODELVIEW );

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//	glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Display::drawModel() {

	glViewport(0, 0, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	glScissor(0, 0, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(70.0f, SCREEN_MARGIN_WIDTH / SCREEN_MARGIN_HEIGHT, 1, 200);

    // Select and setup the modelview matrix
    glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glLoadIdentity();
    gluLookAt( 0.0f, 0.0f, 40.0f,    // Eye-position
               0.0f, 0.0f, 0.0f,   // View-point
               0.0f, 1.0f, 0.0f );  // Up-vector


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mRenderer.render();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

    glMatrixMode( GL_MODELVIEW );

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}

void Display::drawHeightMap() {
	glPushMatrix();
//	imageDrawer(image2);
	glEnable(GL_DEPTH_TEST);
	glRasterPos2f(-1.0f , -0.4f);
	glDrawPixels(mHeightMap.cols, mHeightMap.rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, mHeightMap.data);
	glRasterPos2f(-1.0f, -1.0f);
	glPopMatrix();
}

/**
 * Update lighting according to the direction specified
 */
void Display::updateLight(Display::LightingDirection dir) {
	mDir = dir;
}
/**
 * Update mode according to the height map and texture map provided.
 * Rendering is done through OpenGL over GLFW
 */
void Display::updateModel(Mat heightMap, Mat textureMap) {
//	mRenderer.update(heightMap, textureMap);
	mRenderer.setHeightMap(heightMap);
	mRenderer.setTextureMap(textureMap);
}

/**
 * Display images in OpenCV Mat grayscale format CV_32FC1, range 0 to 1
 */
void Display::updateHeightMap(Mat image) {
	flip(image, mHeightMap, 0);
	mHeightMap *= 255.0;
	mHeightMap.convertTo(mHeightMap, CV_8UC1);
}
