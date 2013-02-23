/*
 * GaussSeidelSolver.h
 *
 *  Created on: Aug 15, 2011
 *      Author: mistralay
 */

#ifndef GAUSSSEIDELSOLVER_H_
#define GAUSSSEIDELSOLVER_H_

#include "opencv2/core/core.hpp"
#include "opencv2/gpu/gpu.hpp"

using namespace cv;

class GaussSeidelSolver {
public:
	GaussSeidelSolver();
	virtual ~GaussSeidelSolver();

	void normalToHeight(Mat normals, int numRow, Mat& height);
//	void normalToHeightGpu(Mat normals, int numRow, Mat& height);



private:
	void normalDifferentialToHeight(Mat xDifferential, Mat yDifferential, Mat& height);
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

//	// gpu
//	void normalDifferentialToHeightGpu(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& height);
////	void computeNormalDifferentialGpu(Mat normalVectors, int imageNumRow, Mat& xDifferentialMat, Mat& yDifferentialMat);
//	void scaleDownDifferentialAndHeightGpu(
//			gpu::GpuMat xDifferential,
//			gpu::GpuMat yDifferential,
//			gpu::GpuMat heightMat,
//			gpu::GpuMat& xDiffSmall,
//			gpu::GpuMat& yDiffSmall,
//			gpu::GpuMat& heightMatSmall);
//	void computeDifferentialAdjustGpu(gpu::GpuMat xDifferential, gpu::GpuMat yDifferential, gpu::GpuMat& adjust);
//	void computeNeighborAdjustGpu(gpu::GpuMat heightMat, gpu::GpuMat& adjust);

};

#endif /* GAUSSSEIDELSOLVER_H_ */
