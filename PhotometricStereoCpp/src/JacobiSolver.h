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

private:
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

	// Used internally by GPU processing. These Mat are made created once only as GpuMat allocation is expensive
	vector<GpuMatPtr > heightGpuMatVector;
	vector<GpuMatPtr > neighborGpuMatVector;
	vector<GpuMatPtr > pqAdjustGpuMatVector;
	vector<GpuMatPtr > oldHeightGpuMatVector;
	vector<GpuMatPtr > diffGpuMatVector;
	vector<GpuMatPtr > xdiffGpuMatVector;
	vector<GpuMatPtr > ydiffGpuMatVector;

	GpuMatPtr getHeightGpuMat(unsigned int level, int numRow, int numCol, int type);
	GpuMatPtr getNeighborGpuMat(unsigned int level, int numRow, int numCol, int type);
	GpuMatPtr getPQAdjustGpuMat(unsigned int level, int numRow, int numCol, int type);
	GpuMatPtr getOldHeightGpuMat(unsigned int level);
	GpuMatPtr getDiffGpuMat(unsigned int level);
	GpuMatPtr getXDiffGpuMat(unsigned int level);
	GpuMatPtr getYDiffGpuMat(unsigned int level);


	void normalDifferentialToHeightGpu(Mat xDifferential, Mat yDifferential, Mat& heightMat, unsigned int level);
	void normalDifferentialToHeightGpuHelper(GpuMatPtr xDifferential, GpuMatPtr yDifferential, GpuMatPtr heightMat, unsigned int level);

	void pyramidDownGpu(GpuMatPtr up, GpuMatPtr down);
	void pyramidUpGpu(GpuMatPtr down, GpuMatPtr up);

};

#endif /* JACOBISOLVER_H_ */
