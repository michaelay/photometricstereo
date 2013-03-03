/*
 * HeightMapRenderer.h
 *
 *  Created on: Mar 3, 2013
 *      Author: michaelay
 */

#ifndef HEIGHTMAPRENDERER_H_
#define HEIGHTMAPRENDERER_H_

#include "Renderer.h"
#include <opencv2/core/core.hpp>

class HeightMapRenderer: public Renderer {
public:
	HeightMapRenderer();
	virtual ~HeightMapRenderer();

	void setHeightMap(cv::Mat heightMap);
	void render();

private:
	cv::Mat mHeightMap;
};

#endif /* HEIGHTMAPRENDERER_H_ */
