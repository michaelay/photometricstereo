/*
 * Display.h
 *
 *  Created on: Aug 20, 2011
 *      Author: mistralay
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "opencv2/core/core.hpp"
#include "renderer/ModelRenderer.h"

using namespace cv;

class Display {
private:
	static const int SCREEN_WIDTH = 1680;
	static const int SCREEN_HEIGHT = 1050;
	static const int SCREEN_MARGIN_WIDTH = 315; // (1680 - 1060) / 2;
	static const int SCREEN_MARGIN_HEIGHT = 315; // (1680 - 1060) / 2;
	ModelRenderer mRenderer;
	Mat mHeightMap;

public:
	static Display* getInstance();

	enum LightingDirection {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

private:
	LightingDirection mDir;
	static Display* mInstance;

public:
	Display();
	virtual ~Display();

	/**
	 * Update lighting according to the direction specified
	 */
	void updateLight(Display::LightingDirection dir);
	/**
	 * Update mode according to the height map and texture map provided.
	 * Rendering is done through OpenGL over GLFW
	 */
	void updateModel(Mat heightMap, Mat textureMap);

	/**
	 * Display images in OpenCV Mat grayscale format CV_32FC1, range 0 to 1
	 */
//	void updateDebug1(Mat image);
	void updateHeightMap(Mat image);
//	void updateDebug3(Mat image);

	void draw();

	void waitKey();



private:

//	void imageDrawer(Mat image);

	void drawLight();
	void drawModel();
	void drawHeightMap();
	void drawDebug1();
	void drawDebug3();

};

#endif /* DISPLAY_H_ */
