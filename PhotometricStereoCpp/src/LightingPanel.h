/*
 * LightingPanel.h
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#ifndef LIGHTINGPANEL_H_
#define LIGHTINGPANEL_H_

#include "constant.h"

#include <vector>
#include <string>
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

typedef vector<Mat > DirectionMatList;

class LightingPanel {
public:
	enum LightingPanelDirection {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};
	static const int NUM_DIRECTION=4;
	static const int ROW=1050;
	static const int COL=1680;

private:
	DirectionMatList mDirectionMatList;
public:
	LightingPanel();
	virtual ~LightingPanel();
	void showDirection(LightingPanelDirection dir);
private:
	void initDirectionMatList();
};

#endif /* LIGHTINGPANEL_H_ */
