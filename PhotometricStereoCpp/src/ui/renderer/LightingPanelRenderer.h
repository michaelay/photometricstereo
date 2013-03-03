/*
 * LightingPanel.h
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#ifndef LIGHTINGPANEL_H_
#define LIGHTINGPANEL_H_

#include "Renderer.h"
#include "../../constant.h"
#include <vector>
#include <string>
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

typedef vector<Mat > DirectionMatList;

class LightingPanelRenderer : public Renderer {
public:
	enum LightingPanelDirection {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};
	static const int NUM_DIRECTION=4;

private:
	LightingPanelDirection mCurrentDirection;
public:
	LightingPanelRenderer();
	virtual ~LightingPanelRenderer();

	void nextDirection();
	void setDirection(LightingPanelDirection dir);
	void render();
};

#endif /* LIGHTINGPANEL_H_ */
