/*
 * GlutDisplay.h
 *
 *  Created on: Mar 2, 2013
 *      Author: michaelay
 */

#ifndef GLUTDISPLAY_H_
#define GLUTDISPLAY_H_

#include <opencv2/core/core.hpp>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#include "renderer/HeightMapRenderer.h"
#include "renderer/ModelRenderer.h"
#include "renderer/LightingPanelRenderer.h"
#include "renderer/NormalRenderer.h"

class GlutDisplay {
public:
	static void setArg(int argc, char** argv);
	static GlutDisplay* getInstance();

	virtual ~GlutDisplay();


	enum DisplayMode {
		MANUAL,
		AUTO_ROTATE
	};

	// Initialize, open window and start GLUT main loop
	void start();


	// Stop GLUT main loop and close window
	void stop();
	void setDisplayMode(GlutDisplay::DisplayMode);
	void setNeedDisplay(bool needDisplay);
	void setCallback(void (*callbackFunc)(void));

	void setHeightMap(cv::Mat heightMap);
	void setTextureMap(cv::Mat textureMap);
	void setLightDirection(LightingPanelRenderer::LightingPanelDirection lightDirection);

	// GLUT event handlers, exposed to be called by global
	void handleDisplay();
	void handleKeyboard(unsigned char key, int x, int y);
	void handleReshape(GLint width, GLint height);
	void handleIdle();

protected:
	GlutDisplay(); // client should call getInstance

private:
	static int _mArgc;
	static char** _mArgv;
	bool _mNeedRedraw;
	void (*mCallbackFunc)(void);
	void initGraphics();
	void buildPopupMenu();

	ModelRenderer* mModelRenderer;
	HeightMapRenderer* mHeightMapRenderer;
	LightingPanelRenderer* mLightingPanelRenderer;
	NormalRenderer* mNormalRenderer;
};

#endif /* GLUTDISPLAY_H_ */
