/*
 * ModelRenderer.h
 *
 *  Created on: Aug 16, 2011
 *      Author: mistralay
 */

#ifndef MODELRENDERER_H_
#define MODELRENDERER_H_

#include "Renderer.h"
#include "../../constant.h"
#include <opencv2/core/core.hpp>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


class ModelRenderer : public Renderer {
private:
	static ModelRenderer* mInstance;
	cv::Mat mHeightMap;
	cv::Mat mTextureMap;

	float mAngle;
	float mSign;
public:
	ModelRenderer();
	virtual ~ModelRenderer();

	void setHeightMap(cv::Mat heightMap);
	void setTextureMap(cv::Mat textureMap);


//	void update(cv::Mat heightMap , cv::Mat textureMap);


//	void initialize();
	virtual void render();
};

#endif /* MODELRENDERER_H_ */
