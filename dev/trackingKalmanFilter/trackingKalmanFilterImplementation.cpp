/* 
  Example use of openCV to track an object using colour segmentation and the Kalman filter
  -----------------------------------------------------------------------------------------
  
  (This is the implementation file: it contains the code for dedicated functions to implement the application.
  These functions are called by client code in the application file. The functions are declared in the interface file.) 

  Adapted from the example provided here http://www.robot-home.it/blog/en/software/ball-tracker-con-filtro-di-kalman/                         
  and code here https://github.com/Myzhar/simple-opencv-kalman-tracker/blob/8df8fb04bd06ef0be06c9b68a79fda6edf3efa1a/source/opencv-kalman.cpp 

  David Vernon
  24 November 2017
*/
 
 
#include "trackingKalmanFilter.h"
                                                                                  
int trackingKalmanFilter(int min_hue, int max_hue, int min_sat, int max_sat) {
 
    // David Vernon ... trace of detected locations
    vector<cv::Point>           detectedLocations;  // store contour of detected locations here
    vector<cv::Point>           predictedLocations; // store contour of predicted locations here
    int start;

    // Camera frame
    cv::Mat frame;
    cv::Mat res;

    // >>>> Kalman Filter
    int stateSize = 6;
    int measSize  = 4;
    int contrSize = 0;

    unsigned int type = CV_32F;
    cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

    cv::Mat state(stateSize, 1, type);  // [  x,   y, v_x, v_y,  w,   h]
                                        // [  x,   y, d_x, d_y,  w,   h]  David Vernon alternative state: shift in position intead of velocity
    cv::Mat meas(measSize, 1, type);    // [z_x, z_y,          z_w, z_h]
 
    // Transition State Matrix A
    // Note: set dT at each processing step! // David Vernon - two alternatives: use dT or use shift in state instead of velocity
    // [ 1 0 dT 0  0 0 ]                     //                if using dT, better to implement this function in a timed thread with constant dT
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    cv::setIdentity(kf.transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
    kf.measurementMatrix.at<float>(0) = 1.0f;
    kf.measurementMatrix.at<float>(7) = 1.0f;
    kf.measurementMatrix.at<float>(16) = 1.0f;
    kf.measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Q
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    kf.processNoiseCov.at<float>(0) = (float) 1e-2;
    kf.processNoiseCov.at<float>(7) = (float) 1e-2;
    kf.processNoiseCov.at<float>(14) = (float) 1e-2; //5.0f;  
    kf.processNoiseCov.at<float>(21) = (float) 1e-2; //5.0f;
    kf.processNoiseCov.at<float>(28) = (float) 1e-2;
    kf.processNoiseCov.at<float>(35) = (float) 1e-2;

    // Measures Noise Covariance Matrix R
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));
    // <<<< Kalman Filter

    // Camera Index
    int idx = 1;

    // Camera Capture
    cv::VideoCapture cap;
    

    // >>>>> Camera Settings
    if (!cap.open(idx)) {
       cout << "Webcam not connected.\n" << "Please verify\n";
       //return EXIT_FAILURE;
       prompt_and_exit(1);
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 768);
    // <<<<< Camera Settings

    cout << "\nHit 'q' to exit...\n";

    char ch = 0;
    double ticks = 0;
    bool found = false;
    int notFoundCount = 0;

    // >>>>> Main loop
    while (ch != 'q' && ch != 'Q') {

        // determine dT ... time since last cycle ... in case we need it
        double precTick = ticks;
        ticks = (double) cv::getTickCount();
        double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds

        // Frame acquisition
        cap >> frame;
        frame.copyTo( res );

        if (found) {
            // >>>> Matrix A
            //kf.transitionMatrix.at<float>(2) = (float) dT;
            //kf.transitionMatrix.at<float>(9) = (float) dT;
            kf.transitionMatrix.at<float>(2) = (float) 1;  // David Vernon ... use delta_x (i.e. shift) for the state instead of v_x (i.e. velocity)
            kf.transitionMatrix.at<float>(9) = (float) 1;  // David Vernon ... use delta_y (i.e. shift) for the state instead of v_y (i.e. velocity)
            // <<<< Matrix A

            cout << "dT:" << endl << dT << endl;

            state = kf.predict();
            cout << "State post:" << endl << state << endl;

            cv::Rect predRect;
            predRect.width = (int) state.at<float>(4);
            predRect.height = (int) state.at<float>(5);
            predRect.x = (int) state.at<float>(0) - predRect.width / 2;
            predRect.y = (int) state.at<float>(1) - predRect.height / 2;

            cv::Point center;
            center.x = (int) state.at<float>(0);
            center.y = (int) state.at<float>(1);
            cv::circle(res, center, 2, CV_RGB(255,0,0), -1);

            cv::rectangle(res, predRect, CV_RGB(255,0,0), 2);

            /* David Vernon ... draw trace of predicted points */
            predictedLocations.push_back(center); 
            start = predictedLocations.size()- NUMBER_OF_POINTS_TO_DRAW; // draw the last n points
            if (start < 0)
               start = 0;
            for (unsigned int i=start; i<predictedLocations.size()-1; i++) {
               cv::line(res,predictedLocations[i], predictedLocations[i+1],CV_RGB(200,0,0));
            }
            /* ... draw trace of predicted points */
        }

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
        cv::inRange(frmHsv, cv::Scalar(min_hue, min_sat, 80),           // David Vernon: use parameter values for hue instead of hard-coded values
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
        for (size_t i = 0; i < contours.size(); i++) {
            cv::RotatedRect bBox;
            bBox = cv::minAreaRect(contours[i]);

            /* track balls ... with appox. square bounding box ... this is the original application */
            
			printf("%f", bBox.size.area());

            //Searching for a bBox almost square
            if (bBox.size.area() >= 1000)
            {
                balls.push_back(contours[i]);
                ballsBox.push_back(bBox);
            }
            

            /* David Vernon: track any shape */
            //balls.push_back(contours[i]);
            //ballsBox.push_back(bBox);
        }
        // <<<<< Filtering

        cout << "Balls found:" << ballsBox.size() << endl;

        // >>>>> Detection result
        for (size_t i = 0; i < balls.size(); i++)  {

            cv::drawContours(res, balls, i, CV_RGB(20,150,20), 1);
			//cv::rectangle(res, ballsBox[i]., CV_RGB(0,255,0), 2);

			Point2f bounding_rect_points[4];

			ballsBox[i].points(bounding_rect_points);

			line(res, bounding_rect_points[0], bounding_rect_points[1], Scalar(0, 0, 127));
			line(res, bounding_rect_points[1], bounding_rect_points[2], Scalar(0, 0, 127));
			line(res, bounding_rect_points[2], bounding_rect_points[3], Scalar(0, 0, 127));
			line(res, bounding_rect_points[3], bounding_rect_points[0], Scalar(0, 0, 127));	

            cv::Point center;
			center.x = ballsBox[i].center.x;
            center.y = ballsBox[i].center.y;
            cv::circle(res, center, 2, CV_RGB(20,150,20), -1);

            stringstream sstr;
            sstr << "(" << center.x << "," << center.y << ")";
            cv::putText(res, sstr.str(),
                        cv::Point(center.x + 3, center.y - 3),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20,150,20), 2);

            /* David Vernon ... draw trace of detected points */
            detectedLocations.push_back(center);  
            start = detectedLocations.size()- NUMBER_OF_POINTS_TO_DRAW; // draw the last n points
            if (start < 0)
               start = 0;
            for (unsigned int i=start; i<detectedLocations.size()-1; i++) {
               cv::line(res,detectedLocations[i], detectedLocations[i+1],CV_RGB(0,200,0));
            }
            /* ... draw trace of detected points */
        }
        // <<<<< Detection result

        // >>>>> Kalman Update
        if (balls.size() == 0) {
            notFoundCount++;
            cout << "notFoundCount:" << notFoundCount << endl;
            if( notFoundCount >= 100 ) {
                found = false;
            }
            detectedLocations.clear(); // David Vernon - lost ball so reset the contour of detected locations 
        }
        else {
            notFoundCount = 0;

			meas.at<float>(0) = (float) (ballsBox[0].center.x);
			meas.at<float>(1) = (float)(ballsBox[0].center.y);
            meas.at<float>(2) = (float)ballsBox[0].size.width;
            meas.at<float>(3) = (float)ballsBox[0].size.height;

            if (!found) { // First detection!
                // >>>> Initialization
                kf.errorCovPre.at<float>(0)  = (float) 1; // px
                kf.errorCovPre.at<float>(7)  = (float) 1; // px
                kf.errorCovPre.at<float>(14) = (float) 1;
                kf.errorCovPre.at<float>(21) = (float) 1;
                kf.errorCovPre.at<float>(28) = (float) 1; // px
                kf.errorCovPre.at<float>(35) = (float) 1; // px
                state.at<float>(0)           = meas.at<float>(0);
                state.at<float>(1)           = meas.at<float>(1);
                state.at<float>(2)           = (float) 0;
                state.at<float>(3)           = (float) 0;
                state.at<float>(4)           = meas.at<float>(2);
                state.at<float>(5)           = meas.at<float>(3);
                // <<<< Initialization
                kf.statePost = state;   // Kalman filter state
                found = true;
            }
            else {
                kf.correct(meas);       // Kalman Correction
            }
            cout << "Measure matrix:" << endl << meas << endl;
        }
        // <<<<< Kalman Update

        // Final result
        cv::imshow("Tracking", res);

        // User key
        ch = cv::waitKey(1);
    }
    // <<<<< Main loop
    return EXIT_SUCCESS;
}

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   exit(status);
} 
