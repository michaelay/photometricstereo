/*
 * JacobiSolver.h
 *
 *  Created on: Aug 27, 2011
 *      Author: mistralay
 */

#ifndef JACOBISOLVER_H_
#define JACOBISOLVER_H_

#include "opencv2/core/core.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>

using namespace cv;

typedef Ptr<gpu::GpuMat> GpuMatPtr;

class JacobiSolver {
public:
	JacobiSolver();
	virtual ~JacobiSolver();

	void normalToHeight(Mat normals, int numRow, Mat& height);
	void normalToHeightIterative(Mat normals, int numRow, Mat height);

private:

	std::vector<cv::Mat> mHeightPyramid;
	std::vector<cv::Mat> mHeightPyramid2; // for iteration exit condition comparison
	std::vector<cv::Mat> mNxPyramid;
	std::vector<cv::Mat> mNyPyramid;

	void normalDifferentialToHeight(Mat xDifferential, Mat yDifferential, Mat& height, unsigned int level);
	void computeNormalDifferential(Mat normalVectors, int imageNumRow, Mat& xDifferentialMat, Mat& yDifferentialMat);
	void scaleDownDifferentialAndHeight(
			Mat xDifferential,
			Mat yDifferential,
			Mat heightMat,
			Mat& xDiffSmall,
			Mat& yDiffSmall,
			Mat& heightMatSmall);
	void computeDifferentialAdjust(Mat xDifferential, Mat yDifferential, Mat& adjust);
	void computeNeighborAdjust(Mat heightMat, Mat& adjust);

	void computeNeighborAdjust(gpu::GpuMat heightMat, gpu::GpuMat& adjust);
	void computeDifferentialAdjust(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& adjust);

};

#endif /* JACOBISOLVER_H_ */
