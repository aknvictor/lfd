#include "appImplementation.h"

void prompt_and_exit(int status)
{
    printf("Press any key to continue and close terminal ... \n");
    getchar();    
    exit(status);
}

float *getObjectPose(InputArray frame, int * segmentation_values)
{
    int min_hue;
    int max_hue;
    int min_sat;
    int max_sat;

    float* pose = new float[6];
    pose[0] = -1.0;
    pose[1] = -1.0;
    pose[2] = -1.0;
    pose[3] = -1.0;
    pose[4] = -1.0;
    pose[5] = -1.0;

    min_hue = segmentation_values[0];
    max_hue = segmentation_values[1];
    min_sat = segmentation_values[2];
    max_sat = segmentation_values[3];

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
        printf("found: %lu \n", ballsBox.size());
        //object 1
        pose[0] = 20 * (ballsBox[0].center.x  / 640);    
        pose[1] = 20 * (ballsBox[0].center.y  / 480);   
        pose[2] = 20 * ((ballsBox[0].angle + 360) / (720));

        if(ballsBox.size()> 1)
        {
            
            //object 2
            pose[3] = 20 * (ballsBox[1].center.x  / 640);    
            pose[4] = 20 * (ballsBox[1].center.y  / 480);   
            pose[5] = 20 * ((ballsBox[1].angle + 360) / (720));
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


int timediff(struct timespec end, struct timespec start)
{
    int a = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
    return a;
}

//scale -350:350 into respective scales
float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz)
{
    float *poseDelta = (float *)malloc(sizeof(float) * 5); //should be 4. - x, y, z theta

    poseDelta[0] = 0.0;
    poseDelta[1] = 0.0;
    poseDelta[2] = 0.0;
    poseDelta[3] = 0.0;
    poseDelta[4] = 0.0;

    float f = (1.0 / 350.0);

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

//space nav

void sig(int s)
{
    #if DEMO
    printf("%s", "exiting");
    spnav_close();
    #endif
    exit(0);
}

void loadCameraModel(float cameraModel[][4])
{
    
    ifstream inFile;
    inFile.open("applicationControl/cameraModelCoefficients.txt");

    string line;

    for (int i=0; i<3; i++) {
        getline(inFile, line);

        char *dup = strdup(line.c_str());
        char *tokens = strtok(dup, " ");

        cameraModel[i][0] = atof(dup);
        tokens = strtok(NULL, " ");

        cameraModel[i][1] = atof(tokens);
        tokens = strtok(NULL, " ");

        cameraModel[i][2] = atof(tokens);
        tokens = strtok(NULL, " ");

        cameraModel[i][3] = atof(tokens);
   }

}

void hyptrain()
{
    init();

    ifstream inFile;
    inFile.open("applicationData/trainingdata.txt");
    
    float _da, _dx, _dy, _dz, _g, _dfx, _dfy, _dfz, _dfa;

    string line;

    bool actionread = false;

    Event_t *events = new Event_t();
    EventUnion e;

    while (getline(inFile, line))
    {
        // cout << "\n";
        if (line != "end")
        {
            if(line == "finish")
            {
                train(events, 0, getEventSeqLen(events) - 1);
                events = new Event_t();

                continue;
            }

            char *dup = strdup(line.c_str());
            char *tokens = strtok(dup, " ");

            if (!actionread)
            {            
                //read action

                _dx = atof(dup);
                tokens = strtok(NULL, " ");

                _dy = atof(tokens);
                tokens = strtok(NULL, " ");

                _dz = atof(tokens);
                tokens = strtok(NULL, " ");

                _da = atof(tokens);
                tokens = strtok(NULL, " ");

                _g = atof(tokens);

                actionread = true;

                // printf("%f %f %f %f %f ", _dx, _dy, _dz, _da, _g);
                
                e.action.deltaangle = 0.0;
                e.action.deltaX = _dx;
                e.action.deltaY = _dy;
                e.action.deltaZ = _dz;
                e.action.grasp = _g;

                push(events, e, 1);

            }
            else
            {

                //read obseration

                actionread = false;

                _dfx = atof(dup);
                tokens = strtok(NULL, " ");

                _dfy = atof(tokens);
                tokens = strtok(NULL, " ");

                _dfz = atof(tokens);
                tokens = strtok(NULL, " ");

                _dfa = atof(tokens);
                tokens = strtok(NULL, " "); 
                
                _g = atof(tokens);
                tokens = strtok(NULL, " ");

                // printf("%f %f %f %f ", _dfx, _dfy, _dfz, _dfa);
                
                e.observation.diffZ = (_dfz);
                e.observation.diffY = (_dfy);
                e.observation.diffX = (_dfx);
                e.observation.diffangle = (0.0);
                e.observation.grasp = (_g);

                push(events, e, 2);

            }

            free(dup);
        }
        else
            break;
    }

    
    // train(events, 0, getEventSeqLen(events) - 1);

    delete events;

}

