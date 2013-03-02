/*
 * ModelRenderer.h
 *
 *  Created on: Aug 16, 2011
 *      Author: mistralay
 */

#ifndef MODELRENDERER_H_
#define MODELRENDERER_H_

#include "opencv2/core/core.hpp"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


using namespace cv;

class ModelRenderer {
private:
	static ModelRenderer* mInstance;
	Mat mHeightMap;
	Mat mTextureMap;

	float mAngle;
	float mSign;
public:
	ModelRenderer();
	virtual ~ModelRenderer();

	void update(Mat heightMap , Mat textureMap);

//	static void displayWrapper();
//	static void idleWrapper();
//	static void setInstance(ModelRenderer* instance);
//
//	static void onOpenGL(void* userdata);

	void initialize();
	void display();
//	void idle();

//	void start(int argc, char *argv[]);
//	void start();
//	void registerWindow();
};

#endif /* MODELRENDERER_H_ */
