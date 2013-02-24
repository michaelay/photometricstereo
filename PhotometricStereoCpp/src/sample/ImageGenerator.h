/*
 * ImageGenerator.h
 *
 *  Created on: Feb 24, 2013
 *      Author: michaelay
 */

#ifndef SAMPLEIMAGEGENERATOR_H_
#define SAMPLEIMAGEGENERATOR_H_

#include <string>

using namespace std;

class SampleImageGenerator {
public:
	SampleImageGenerator();
	virtual ~SampleImageGenerator();
	void generateImage(int argc, char **argv, const string& prefix);
	//static void display();
};

#endif /* SAMPLEIMAGEGENERATOR_H_ */
