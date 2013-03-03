/*
 * GlutDisplay.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: michaelay
 */

#include "GlutDisplay.h"
#include "../constant.h"

// default value for static variables
int GlutDisplay::_mArgc = 0;
char** GlutDisplay::_mArgv = NULL;

// Global function for glut event callback
// reference: http://www.parashift.com/c++-faq-lite/memfnptr-vs-fnptr.html
void glutDisplayHandleDisplay() {
	GlutDisplay::getInstance()->handleDisplay();
}

void glutDisplayHandleReshape(GLint width, GLint height) {
	GlutDisplay::getInstance()->handleReshape(width, height);
}

void glutDisplayHandleKeyboard(unsigned char key, int x, int y) {
	GlutDisplay::getInstance()->handleKeyboard(key, x, y);
}

void glutDisplayHandleIdle() {
	GlutDisplay::getInstance()->handleIdle();
}

// Class implementation
void
GlutDisplay::setArg(int argc, char** argv) {
	_mArgc = argc;
	_mArgv = argv;
}

GlutDisplay*
GlutDisplay::getInstance() {
	static GlutDisplay* _instance = NULL;
	if (_instance == NULL) {
		_instance = new GlutDisplay();
	}
	return _instance;
}

GlutDisplay::GlutDisplay() {
	_mNeedRedraw = false;
	mCallbackFunc = NULL;

	mModelRenderer = new ModelRenderer();
	mHeightMapRenderer = new HeightMapRenderer();
	mLightingPanelRenderer = new LightingPanelRenderer();
	mNormalRenderer = new NormalRenderer();

	// GLUT Window Initialization:
	glutInit(&_mArgc, _mArgv);
	if (FULL_SCREEN) {
		glutFullScreen();
	} else {
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("Photometric stereo");
}

GlutDisplay::~GlutDisplay() {
	delete mModelRenderer;
	delete mHeightMapRenderer;
	delete mLightingPanelRenderer;
	delete mNormalRenderer;
}


/**
 * Initialize, open window and start glut main loop
 */
void
GlutDisplay::start() {
	// Initialize OpenGL graphics state
	initGraphics();
	// Register callbacks:
	glutDisplayFunc(glutDisplayHandleDisplay);
	glutReshapeFunc(glutDisplayHandleReshape);
	glutKeyboardFunc(glutDisplayHandleKeyboard);
	//	  glutMouseFunc (MouseButton);
	//	  glutMotionFunc (MouseMotion);
	glutIdleFunc(glutDisplayHandleIdle);
	// Create our popup menu
	buildPopupMenu();
	glutAttachMenu (GLUT_RIGHT_BUTTON);
	// Turn the flow of control over to GLUT
	glutMainLoop ();
}

/**
 * Stop GLUT main loop and close window
 */
void
GlutDisplay::stop() {

}

void
GlutDisplay::setDisplayMode(GlutDisplay::DisplayMode) {
	_mNeedRedraw = true;
}

void
GlutDisplay::setNeedDisplay(bool needDisplay) {
	_mNeedRedraw = needDisplay;
}

void
GlutDisplay::setCallback(void (*callbackFunc)(void)) {
	mCallbackFunc = callbackFunc;
}

 void
GlutDisplay::setHeightMap(cv::Mat heightMap) {
	mHeightMapRenderer->setHeightMap(heightMap);
	mModelRenderer->setHeightMap(heightMap);
	_mNeedRedraw = true;
}

void
GlutDisplay::setTextureMap(cv::Mat textureMap) {
	mModelRenderer->setTextureMap(textureMap);
	_mNeedRedraw = true;
}

void
GlutDisplay::setLightDirection(LightingPanelRenderer::LightingPanelDirection lightDirection) {
	mLightingPanelRenderer->setDirection(lightDirection);
	_mNeedRedraw = true;

}

// helper methods

void
GlutDisplay::initGraphics() {
	glutInitDisplayMode (GLUT_DOUBLE);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
GlutDisplay::buildPopupMenu() {

}



// GLUT event handlers

void
GlutDisplay::handleDisplay() {
}

void
GlutDisplay::handleKeyboard(unsigned char key, int x, int y) {

}

void
GlutDisplay::handleReshape(GLint width, GLint height) {

}

void
GlutDisplay::handleIdle() {
	if (mCallbackFunc != NULL) {
		mCallbackFunc();
	}
//	if (!_mNeedRedraw) {
//		return;
//	}
	// draw code
	glViewport(SCREEN_MARGIN_WIDTH, 0, SCREEN_WIDTH - SCREEN_MARGIN_WIDTH, SCREEN_HEIGHT);
	glScissor(SCREEN_MARGIN_WIDTH, 0, SCREEN_WIDTH - SCREEN_MARGIN_WIDTH, SCREEN_HEIGHT);
	mLightingPanelRenderer->render();

	glViewport(0, 0, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	glScissor(0, 0, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	mModelRenderer->render();

	glViewport(0, SCREEN_MARGIN_HEIGHT, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	glScissor(0, SCREEN_MARGIN_HEIGHT, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	mNormalRenderer->render();

	glViewport(0, 2*SCREEN_MARGIN_HEIGHT, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	glScissor(0, 2*SCREEN_MARGIN_HEIGHT, SCREEN_MARGIN_WIDTH, SCREEN_MARGIN_HEIGHT);
	mHeightMapRenderer->render();

//	glViewport(SCREEN_MARGIN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//	glScissor(SCREEN_MARGIN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glutSwapBuffers();
	_mNeedRedraw = false;
}



