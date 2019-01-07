#include "spacenavInterface.h"


struct timespec counter, start;

int main(int argc, char **argv)
{

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

        //sample every 500 milliseconds
        if (timediff(counter, start) > 500)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            if (sev.type == SPNAV_EVENT_MOTION)
            {
               printf("Motion detected: x: %d, y: %d, z: %d, rx: %d, ry: %d, rz: %d", sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 0 and sev.button.press)
            {
                printf("Left Button press detected");

                return 0;
                spnav_close();
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 1 and sev.button.press)
            {
                printf("Right Button press detected");
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 0 and !sev.button.press)
            {
                printf("Right Button release detected");
            }
            else if (sev.type == SPNAV_EVENT_BUTTON and sev.button.bnum == 1 and !sev.button.press)
            {
                printf("Right Button release detected");
            }
        }
    }

    spnav_close();
    return 0;
}