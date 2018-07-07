#include "../servoController/controllerInterface.h"
#include "../predictiveSeqLearning/pslImplementation.h"

#define DEBUG 0
#define TRAIN 1
#define CALIBRATE 0
#define DEMO 1
#define CAM_IDX1 0
#define CAM_IDX2 2

using namespace std;
using namespace cv;

void prompt_and_exit(int status);

float *getObjectPose(InputArray frame, int * segmentation_values, float width, float height);

float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz);

int timediff(struct timespec end, struct timespec start);

int open_port(void);

//space nav

void sig(int s);


//psl

void hyptrain();

void loadCameraModel(float cameraModel[][4]);
