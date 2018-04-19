#include "appImplementation.h"

void prompt_and_exit(int status)
{
    printf("Press any key to continue and close terminal ... \n");
    getchar();
    exit(status);
}

float *getObjectPose(InputArray frame)
{
    int min_hue;
    int max_hue;
    int min_sat;
    int max_sat;

    FILE *fp_in;

    float* pose = new float[6];
    pose[0] = -1.0;
    pose[1] = -1.0;
    pose[2] = -1.0;
    pose[3] = -1.0;
    pose[4] = -1.0;
    pose[5] = -1.0;
    
    

    if ((fp_in = fopen("ObjectTrackingInput.txt", "r")) == 0)
    {
        printf("Error can't open input file ObjectTrackingInput.txt\n");
        prompt_and_exit(1);
    }

    fscanf(fp_in, "%d %d %d %d", &min_hue, &max_hue, &min_sat, &max_sat);

    cv::Mat res;
    frame.copyTo(res);

    // >>>>> Noise smoothing
    cv::Mat blur;
    cv::GaussianBlur(frame, blur, cv::Size(5, 5), 3.0, 3.0);
    // <<<<< Noise smoothing

    // >>>>> HSV conversion
    cv::Mat frmHsv;
    cv::cvtColor(blur, frmHsv, CV_BGR2HSV);
    // <<<<< HSV conversion

    // >>>>> Color Thresholding
    // Note: change parameters for different colors
    cv::Mat rangeRes = cv::Mat::zeros(frame.size(), CV_8UC1);

    //cv::inRange(frmHsv, cv::Scalar(MIN_H_BLUE / 2, 100, 80),
    //            cv::Scalar(MAX_H_BLUE / 2, 255, 255), rangeRes);
    cv::inRange(frmHsv, cv::Scalar(min_hue, min_sat, 80), // David Vernon: use parameter values for hue instead of hard-coded values
                cv::Scalar(max_hue, max_sat, 255), rangeRes);
    // <<<<< Color Thresholding

    // >>>>> Improving the result
    cv::erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
    // <<<<< Improving the result

    // Thresholding viewing
    cv::imshow("Threshold", rangeRes);

    // >>>>> Contours detection
    vector<vector<cv::Point> > contours;
    cv::findContours(rangeRes, contours, CV_RETR_EXTERNAL,
                     CV_CHAIN_APPROX_NONE);
    // <<<<< Contours detection

    // >>>>> Filtering
    vector<vector<cv::Point> > balls;
    vector<cv::RotatedRect> ballsBox;
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::RotatedRect bBox;
        bBox = cv::minAreaRect(contours[i]);

        //Searching for a bBox almost square
        if (bBox.size.area() >= 1000)
        {
            balls.push_back(contours[i]);
            ballsBox.push_back(bBox);
        }
    }

    if(ballsBox.size()> 0)
    {
        printf("size: %lu ", ballsBox.size());
        pose[0] = ballsBox[0].center.x;    
        pose[1] = ballsBox[0].center.y;    
        pose[2] = ballsBox[0].angle; 
        
        if(ballsBox.size()> 1)
        {
            pose[3] = ballsBox[1].center.x;    
            pose[4] = ballsBox[1].center.y;    
            pose[5] = ballsBox[1].angle; 
        }
           

    }

    return pose;
}

int open_port(void)
{
    int fd; /* File descriptor for the port */

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        // Could not open the port.
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }
    else
        fcntl(fd, F_SETFL, 0);

    struct termios options;

    tcgetattr(fd, &options);
    //setting baud rates and stuff
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    tcsetattr(fd, TCSAFLUSH, &options);

    options.c_cflag &= ~PARENB; //next 4 lines setting 8N1
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    //options.c_cflag &= ~CNEW_RTSCTS;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //raw input

    options.c_iflag &= ~(IXON | IXOFF | IXANY); //disable software flow control

    sleep(2); //required to make flush work, for some reason
    tcflush(fd, TCIOFLUSH);

    return (fd);
}

void sig(int s)
{
    spnav_close();
    exit(0);
}

int timediff(struct timespec end, struct timespec start)
{

    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

//scale -350:350 into respective scales
float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz)
{
    float *poseDelta = (float *)malloc(sizeof(float) * 5); //should be 4. - x, y, z theta

    float f = (1.0 / 35.0);

    float x = f * m_x;
    float y = f * m_z;
    float z = f * m_y;
    float p = f * m_rx;
    float r = f * m_rz; // in the endeffector FOR, the roll is rotation about z when facing direction of wrist. when facing downward its rotation about x in the robot FOR, the roll is a rotation about y when in the wrist direction and about z when facing down

    // printf("\n \nMouse: \n %d %d %d %d %d %d \n", m_x, m_y, m_z, m_rx, m_ry, m_rz);
    // printf("Pose delta: \n %f %f %f %f %f \n", x, y, z, p, r);

    poseDelta[0] = x;
    poseDelta[1] = y;
    poseDelta[2] = z;
    poseDelta[3] = p;
    poseDelta[4] = r;

    return poseDelta;
}
