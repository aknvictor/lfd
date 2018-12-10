#include "controllerInterface.h"
struct timespec counter, start;

int main(int argc, char **argv)
{

    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9" on linux "/dev/ttyUSB0;
    char BAUD[7] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    int x = 0;
    int y = 200;
    int z = 120;
    int pitch = -180;
    int roll = -90;
    int graspVal = 30;

    readRobotConfigurationData("../applicationControl/robotConfig.txt");

    goHome();

    printf("%d", gotoPose(x, y, z, pitch, roll));

    spnav_event sev;
    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to the space navigator daemon\n");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (spnav_wait_event(&sev))
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &counter);

        if (timediff(counter, start) > 500) //sample every 200 milliseconds or 5Hz
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            if (sev.type == SPNAV_EVENT_MOTION)
            {
                float *poseDelta = scale_and_map(sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);
                x = poseDelta[0] + x;
                y = poseDelta[1] + y;
                z = poseDelta[2] + z;
                pitch = poseDelta[3] + pitch;
                roll = poseDelta[4] + roll;
                gotoPose(x, y, z, pitch, roll);
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 0 and sev.press)
            {
                return 0;
                spnav_close();
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 1 and sev.press)
            {
                grasp(graspVal);
                graspVal = abs(graspVal - 30)
            }
        }
    }

    spnav_close();
    return 0;
}