/*
 * VTKModelRenderer.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: michael.auyeung
 */

#include "VTKModelRenderer.h"

VTKModelRenderer::VTKModelRenderer() {
	// TODO Auto-generated constructor stub
}

VTKModelRenderer::~VTKModelRenderer() {
	// TODO Auto-generated destructor stub
}

void
VTKModelRenderer::setHeightMap(cv::Mat heightMap) {
	mHeightMap = heightMap;
}

void
VTKModelRenderer::setTextureMap(cv::Mat textureMap) {
	mTextureMap = textureMap;
}

void
VTKModelRenderer::render() {

    int height = mHeightMap.rows;
    int width = mHeightMap.cols;

    /* creating visualization pipeline which basically looks like this:
     vtkPoints -> vtkPolyData -> vtkPolyDataMapper -> vtkActor -> vtkRenderer */
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyDataMapper> modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> modelActor = vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkCellArray> vtkTriangles = vtkSmartPointer<vtkCellArray>::New();

    /* insert x,y,z coords */
    for (int y=0; y < height; y++) {
        for (int x=0; x < width; x++) {
            points->InsertNextPoint(x, y, mHeightMap.at<float>(y,x));
        }
    }

    /* setup the connectivity between grid points */
    vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
    triangle->GetPointIds()->SetNumberOfIds(3);
    /* setup the connectivity between grid points */
    for (int i=0; i<height-1; i++) {
        for (int j=0; j<width-1; j++) {
            triangle->GetPointIds()->SetId(0, j+(i*width));
            triangle->GetPointIds()->SetId(1, (i+1)*width+j);
            triangle->GetPointIds()->SetId(2, j+(i*width)+1);
            vtkTriangles->InsertNextCell(triangle);
            triangle->GetPointIds()->SetId(0, (i+1)*width+j);
            triangle->GetPointIds()->SetId(1, (i+1)*width+j+1);
            triangle->GetPointIds()->SetId(2, j+(i*width)+1);
            vtkTriangles->InsertNextCell(triangle);
        }
    }
    polyData->SetPoints(points);
    polyData->SetPolys(vtkTriangles);


    /* create two lights */
    vtkSmartPointer<vtkLight> light1 = vtkSmartPointer<vtkLight>::New();
    light1->SetPosition(-1, 1, 1);
    renderer->AddLight(light1);
    vtkSmartPointer<vtkLight> light2 = vtkSmartPointer<vtkLight>::New();
    light2->SetPosition(1, -1, -1);
    renderer->AddLight(light2);

    /* meshlab-ish background */
    modelMapper->SetInput(polyData);
    renderer->SetBackground(.45, .45, .9);
    renderer->SetBackground2(.0, .0, .0);
    renderer->GradientBackgroundOn();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    modelActor->SetMapper(modelMapper);

    /* setting some properties to make it look just right */
    modelActor->GetProperty()->SetSpecularColor(1, 1, 1);
    modelActor->GetProperty()->SetAmbient(0.2);
    modelActor->GetProperty()->SetDiffuse(0.2);
    modelActor->GetProperty()->SetInterpolationToPhong();
    modelActor->GetProperty()->SetSpecular(0.8);
    modelActor->GetProperty()->SetSpecularPower(8.0);

    renderer->AddActor(modelActor);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    /* render mesh */
    renderWindow->Render();
    interactor->Start();
}
