/*
 * ImageGenerator.cpp
 *
 *  Created on: Feb 24, 2013
 *      Author: michaelay
 */

#include "ImageGenerator.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <sstream>
#include <math.h>
#include <sys/time.h>
#include "../constant.h"

#define NUM_IMAGE 4
//#define ILUT_USE_OPENGL
#include <IL/devil_cpp_wrapper.hpp>

using namespace std;

#define PI 3.14159265

void takeScreenshot(const char* screenshotFile)
{
    ILuint imageID = ilGenImage();
    ilBindImage(imageID);
    ilutGLScreen();
    ilEnable(IL_FILE_OVERWRITE);
    ilSaveImage(screenshotFile);
    ilDeleteImage(imageID);
    printf("Screenshot saved to: %s\n", screenshotFile);
}

//void idle() {
////	long speedFactor = 5000.0;
//	long ts;
//
//	struct timeval current;
//	gettimeofday(&current, NULL);
//
//	ts = current.tv_usec;
////	long angle = ts / (2*PI);
//	float angle = 2.0 * PI / NUM_IMAGE * (ts % NUM_IMAGE);
//	cout << "angles: " << angle << " : " << cos(angle) << " : " << sin(angle) << " : " <<  ts % NUM_IMAGE << " : " << 2.0 * PI / NUM_IMAGE << endl;
//	GLfloat light_position[] = { cos(angle), sin(angle), -2.0, 0.0 };
//
//    glMatrixMode(GL_MODELVIEW);
//    // clear the drawing buffer.
//    glClear(GL_COLOR_BUFFER_BIT);
//    // clear the identity matrix.
//    glLoadIdentity();
//    // translate the draw by z = -4.0
//    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
//    glTranslatef(0.0,0.0,-4.0);
//    // Red color used to draw.
//    glColor3f(0.9, 0.3, 0.2);
//    // scaling transfomation
//    // built-in (glut library) function , draw you a sphere.
//    GLdouble radius = 1;
//    glutSolidSphere(radius,20,20);
//    // Flush buffers to screen
//
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//
//    glFlush();
//    // swap buffers called because we are using double buffering
//   // glutSwapBuffers();
//
//    ostringstream oss;
//    oss << "screenshot_" << angle << ".png";
//    string filename = oss.str();
//    takeScreenshot(filename.c_str());
//}

void drawImage(float angle) {
	GLfloat light_position[] = { cos(angle), sin(angle), -2.0, 0.0 };

    glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    // clear the identity matrix.
    glLoadIdentity();
    // translate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    glTranslatef(0.0,0.0,-4.0);
    // Red color used to draw.
    glColor3f(0.9, 0.3, 0.2);
    // scaling transfomation
    // built-in (glut library) function , draw you a sphere.
    GLdouble radius = 1;
    glutSolidSphere(radius,20,20);
    // Flush buffers to screen

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glFlush();
    // swap buffers called because we are using double buffering
   // glutSwapBuffers();
}

void idle() {
	// draw all images and save
	float angle = 0;
	for (int i=0; i<NUM_IMAGE; i++) {
		angle = 2.0 * PI / NUM_IMAGE * i;
		drawImage(angle);
		// save image
	    ostringstream oss;
	    oss << "screenshot_" << i << ".png";
	    string filename = oss.str();
	    takeScreenshot(filename.c_str());

	}
}


void display()
{
    glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    // clear the identity matrix.
    glLoadIdentity();
    // translate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    glTranslatef(0.0,0.0,-4.0);
    // Red color used to draw.
    glColor3f(0.9, 0.3, 0.2);
    // scaling transfomation
    // built-in (glut library) function , draw you a sphere.
    GLdouble radius = 1;
    glutSolidSphere(radius,20,20);
    // Flush buffers to screen

    glFlush();
    // swap buffers called because we are using double buffering
   // glutSwapBuffers();
}

void reshape(int x, int y)
{
    if (y == 0 || x == 0) return;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(39.0,(GLdouble)x/(GLdouble)y,0.6,21.0);
    gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.9,21.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

SampleImageGenerator::SampleImageGenerator() {
	// TODO Auto-generated constructor stub
	ilInit();
//	iluInit();
//	ilutRenderer(ILUT_OPENGL);
}

SampleImageGenerator::~SampleImageGenerator() {
	// TODO Auto-generated destructor stub
}

void
SampleImageGenerator::generateImage(int argc, char **argv, const string& prefix) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(CAPTURE_WIDTH,CAPTURE_HEIGHT);
    glutCreateWindow("Here is a sphere, for some reason");

    // make sphere shinny
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

	//    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { sqrt(2.0), sqrt(2.0), -2.0, 0.0 };

	//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutKeyboardFunc(keyb);
    glutIdleFunc(idle);

    glClearColor(0.0,0.0,0.0,0.0);
    glutMainLoop();
}


