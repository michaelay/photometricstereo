/*
 * VTKModelRenderer.h
 *
 *  Created on: Mar 7, 2013
 *      Author: michael.auyeung
 */

#ifndef VTKMODELRENDERER_H_
#define VTKMODELRENDERER_H_

#include "Renderer.h"
#include "opencv2/core/core.hpp"
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageViewer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkTriangle.h>

class VTKModelRenderer: public Renderer {
public:
	VTKModelRenderer();
	virtual ~VTKModelRenderer();

	void setHeightMap(cv::Mat heightMap);
	void setTextureMap(cv::Mat textureMap);

	void incrementAngle(bool x, bool y) {}
	void decrementAngle(bool x, bool y) {}

	virtual void render();

private:
	cv::Mat mHeightMap;
	cv::Mat mTextureMap;
};

#endif /* VTKMODELRENDERER_H_ */
