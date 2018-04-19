#include "controllerInterface.h"

#define DEBUG 0
#define CALIBRATE 0
#define DEMO 1

using namespace std;
using namespace cv;

void prompt_and_exit(int status);

float *getObjectPose(InputArray frame);

float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz);

int timediff(struct timespec end, struct timespec start);

void sig(int s);

int open_port(void);