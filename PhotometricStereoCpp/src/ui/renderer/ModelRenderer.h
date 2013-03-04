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
	static const float ANGLE_STEP_SIZE = 3.0f;
	static const float ANGLE_MIN = -45.0f;
	static const float ANGLE_MAX = 45.0f;
	static const float ANGLE_DEFAULT = 0.0f;

	static ModelRenderer* mInstance;
	cv::Mat mHeightMap;
	cv::Mat mTextureMap;

	float mXAngle;
	float mYAngle;

//	float mSign;

	void updateAngle(float& angle, bool increment);

public:
	ModelRenderer();
	virtual ~ModelRenderer();

	void setHeightMap(cv::Mat heightMap);
	void setTextureMap(cv::Mat textureMap);

	void incrementAngle(bool x, bool y);
	void decrementAngle(bool x, bool y);

	virtual void render();
};

#endif /* MODELRENDERER_H_ */
