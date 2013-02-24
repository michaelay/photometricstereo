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
//    // changing in transformation matrix.
//    GLfloat xRotated = 43.0;
//    GLfloat yRotated = 50.0;
//    GLfloat zRotated = 30.0;
//    // rotation about X axis
//    glRotatef(xRotated,1.0,0.0,0.0);
//    // rotation about Y axis
//    glRotatef(yRotated,0.0,1.0,0.0);
//    // rotation about Z axis
//    glRotatef(zRotated,0.0,0.0,1.0);
    // scaling transfomation
    glScalef(1.0,1.0,1.0);
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
    gluPerspective(39.0,(GLdouble)x/(GLdouble)y,0.6,21.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

SampleImageGenerator::SampleImageGenerator() {
	// TODO Auto-generated constructor stub

}

SampleImageGenerator::~SampleImageGenerator() {
	// TODO Auto-generated destructor stub
}

void
SampleImageGenerator::generateImage(int argc, char **argv, const string& prefix) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutCreateWindow("Here is a sphere, for some reason");

    // make sphere shinny
//    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, -2.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutKeyboardFunc(keyb);
    glClearColor(0.0,0.0,0.0,0.0);
    glutMainLoop();
}

