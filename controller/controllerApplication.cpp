#include "controllerInterface.h"

int main(int argc, char ** argv)
{

    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9" on linux "/dev/ttyUSB0;
    char BAUD[7] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    float x = 80;
    float y = 200;
    float z = 150;
    float pitch = -90;
    float roll = 0;
    int graspVal = 60;

    initializeControllerWithSpeed(PORT, BAUD, speed);

    gotoPose(x, y, z, pitch, roll);

    return 0;

}