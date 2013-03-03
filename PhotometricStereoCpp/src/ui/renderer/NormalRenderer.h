/*
 * NormalRenderer.h
 *
 *  Created on: Mar 3, 2013
 *      Author: michaelay
 */

#ifndef NORMALRENDERER_H_
#define NORMALRENDERER_H_

#include "Renderer.h"
#include <opencv2/core/core.hpp>

class NormalRenderer: public Renderer {
public:
	NormalRenderer();
	virtual ~NormalRenderer();

	void setNormal(cv::Mat normal);
	void render();

private:
	cv::Mat mNormal;
};

#endif /* NORMALRENDERER_H_ */
