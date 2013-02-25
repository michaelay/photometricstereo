#ifndef __CONSTANT_H__
#define __CONSTANT_H__

//#define WIN
#define MAC

#include <string>

#define TIME_CHECK
//#define USE_GPU

#define NUM_IMAGES 4
#define CAPTURE_WIDTH 320
#define CAPTURE_HEIGHT 240
#define CAPTURE_FPS 50
#define ROI_THRESHOLD 0.05

#define MAX_NUM_ITERATION 500
#define GAUSS_SEIDEL_THRESHOLD 0.0001 // 0.0001
#define MIN_ROW_TO_SCALE 8

#define RECORD_VIDEO false

const bool MODEL_ROTATE = true;
const float MODEL_ANGLE_MIN = -45.0f;
const float MODEL_ANGLE_MAX = 45.0f;
const float MODEL_ANGLE_STEP = 3.0f;

//const float MODEL_ANGLE_MIN = 25.0f;

#define WEBCAM_NUMBER 1

const int HEIGHT_MAP_SCALE = 3000; //6000;
#ifdef WIN
const int WEBCAM_NUMBER_FLUSH = 1;
const bool FLIP_HEIGHT = false;
#else
const int WEBCAM_NUMBER_FLUSH = 0;
const bool FLIP_HEIGHT = false;
#endif

const long CAPTURE_WAIT_MILLISECOND = 100; //130;
const int CAPTURE_IDX_OFFSET = 0; // -1 to 2

#define ADJUSTMENT_MAT_FILENAME "Adjustment.yml"
#define ADJUSTMENT_MAT_KEY "adj"

#define SHOW_ROI false
#define SHOW_NORMAL true
#define SHOW_HEIGHTMAP true
#define INTERACTIVE

#define MODEL_WINDOW_WIDTH 320
#define MODEL_WINDOW_HEIGHT 240
#define MODEL_WINDOW_TITLE "Model"
#define MODEL_WINDOW_FIELD_OF_VIEW_ANGLE 45
#define MODEL_WINDOW_Z_NEAR 1.0f
#define MODEL_WINDOW_Z_FAR 500.0f

//const std::string SAVE_FILENAME_PREFIX = "image_";
const std::string SAVE_FILENAME_PREFIX = "ss_";

const std::string WINDOW_LIGHTING = "Lighting";
const std::string WINDOW_OUTPUT = "Output";
const std::string WINDOW_DEBUG = "Debug";
const std::string WINDOW_MODEL = "Model";

const std::string BUTTON_STATIC = "Static";
const std::string BUTTON_VIDEO = "Video";
const std::string BUTTON_CAM = "Use camera 2";

#endif
