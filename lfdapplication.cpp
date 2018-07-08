//controller.cpp

#include "lfdApplication/appImplementation.h"
#include "servoController/controllerInterface.h"
#include "cameraInvPerspectiveMonocular/cameraInvPerspectiveMonocular.h"


struct timespec counter, start, pressKey, releaseKey;

int main()
{

    float camera_model[3][4];
    loadCameraModel(camera_model);

    Point2f imagePoint;
    Point3f worldPoint;

    readRobotConfigurationData("applicationControl/robotConfig.txt");

    float x = 0;
    float y = 120;
    float z = 200;
    float pitch = -180;
    float roll = -90;
    int graspVal = 0; //open

    goHome();
    gotoPose(x, y, z, pitch, roll);
    grasp(0);

    FILE *fp_in;
    if ((fp_in = fopen("applicationControl/objectTrackingInput.txt", "r")) == 0)
    {
        printf("Error can't open input file objectTrackingInput.txt\n");
        prompt_and_exit(1);
    }

    int *segmentation_values = new int[4];
    int min_hue;
    int max_hue;
    int min_sat;
    int max_sat;

    fscanf(fp_in, "%d %d %d %d", &min_hue, &max_hue, &min_sat, &max_sat);

    segmentation_values[0] = min_hue;
    segmentation_values[1] = max_hue;
    segmentation_values[2] = min_sat;
    segmentation_values[3] = max_sat;

    FILE *fp_in2;
    if ((fp_in2 = fopen("applicationControl/controlInput.txt", "r")) == 0)
    {
        printf("Error can't open input file controlInput.txt\n");
        prompt_and_exit(1);
    }

    // Camera Index
    int idx = CAM_IDX1;

    // Camera Capture
    cv::VideoCapture cap;
    cv::Mat frame;

    if (!cap.open(idx))
    {
        cout << "Webcam not connected.\n"
             << "Please verify\n";

        prompt_and_exit(1);
    }

    
    float width = cap.get(CV_CAP_PROP_FRAME_WIDTH) ; 
    float height = cap.get(CV_CAP_PROP_FRAME_HEIGHT) ;

#if DEMO

    printf("\n %s \n", "Commence demonstration"); 

    FILE *training_file = fopen("applicationData/trainingdata.txt", "a");
    FILE *training_file_2 = fopen("applicationData/trainingdata2.txt", "a");
    

    if (training_file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

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
                //delta (x, y, z, theta)
                float *poseDelta = scale_and_map(sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);

                //take the max delta only and act on only that axis

                int index = 0;
                if (fabs(poseDelta[0]) < fabs(poseDelta[1]))
                    index = 1;
                if (fabs(poseDelta[index]) < fabs(poseDelta[2]))
                    index = 2;
                if (fabs(poseDelta[index]) < fabs(poseDelta[3]))
                    index = 3;
                if (fabs(poseDelta[index]) < fabs(poseDelta[4]))
                    index = 4;

                int status = 0;
                if (index == 0)
                    status = gotoPose(x + (poseDelta[0] < 0.0 ? -3 : 3) , y, z, pitch, roll);

                else if (index == 1)
                    status = gotoPose(x, y + (poseDelta[1] < 0.0 ? -3 : 3), z, pitch, roll);

                else if (index == 2)
                    status = gotoPose(x, y, z + (poseDelta[2] < 0.0 ? -3 : 3), pitch, roll);

                else if (index == 4)
                    // status = gotoPose(x, y, z, pitch, roll);
                    status = gotoPose(x, y, z, pitch, roll + (poseDelta[4] < 0.0 ? -3 : 3));

                //printf("\n status: %d \n", status );
                if (status)
                {
                    //Action: posedeltas

                    if (index == 0)
                        x += (poseDelta[0] < 0.0 ? -3 : 3);
                    else if (index == 1)
                        y += (poseDelta[1] < 0.0 ? -3 : 3);
                    else if (index == 2)
                        z += (poseDelta[2] < 0.0 ? -3 : 3);
                    else if (index == 4)
                        roll += (poseDelta[4] < 0.0 ? -3 : 3);
                        // roll;
                        
                    // Observation: objectpose - endeffector pose

                    cap >> frame;

                    //camera 1 (down ward facing)
                    float *ff = getObjectPose(frame, segmentation_values, width, height );
                    float *objectpose = new float[4]; //delta (x, y, z, theta)
                    
                   imagePoint.x = ff[0];
                   imagePoint.y = ff[1];

                   inversePerspectiveTransformation(imagePoint, camera_model, 0, &worldPoint);
                    
                    objectpose[0] = x - worldPoint.x;
                    objectpose[1] = y -  worldPoint.y;
                    objectpose[2] = z - worldPoint.z;

                    printf("\n Angle: %f \n", ff[2]);
                    objectpose[3] = roll - ff[2];


                    if (objectpose[0] != -1.0 && !(poseDelta[0] == 0.0 && poseDelta[1] == 0.0 && poseDelta[2] == 0.0 && poseDelta[3] == 0.0 && poseDelta[4] == 0.0))
                    {
                        
                        printf( " %d %d %d %d %d\n", index == 0 ? poseDelta[0] < 0.0 ? -3 : 3 : 0, index == 1 ? poseDelta[1] < 0.0 ? -3 : 3 : 0, index == 2 ? poseDelta[2] < 0.0 ? -3 : 3 : 0, index == 4 ? poseDelta[4] < 0.0 ? -3 : 3 : 0, (int) graspVal);
                        fprintf(training_file_2, " %d %d %d %d %d\n", index == 0 ? poseDelta[0] < 0.0 ? -3 : 3 : 0, index == 1 ? poseDelta[1] < 0.0 ? -3 : 3 : 0, index == 2 ? poseDelta[2] < 0.0 ? -3 : 3 : 0, index == 4 ? poseDelta[4] < 0.0 ? -3 : 3 : 0, (int) graspVal);

                        //endeffector and object differential pose

                        objectpose[0] += 0.5;
                        objectpose[1] += 0.5;
                        objectpose[2] += 0.5;
                        objectpose[3] += 0.5;

                        printf(" %d %d %d %d %d\n", (int) objectpose[0], (int) objectpose[1], (int) objectpose[2], (int) objectpose[3], (int) graspVal);
                        fprintf(training_file_2, " %d %d %d %d %d\n", (int) (objectpose[0]/2.0), (int) (objectpose[1]/2.0), (int) (objectpose[2]/2.0), (int) (objectpose[3]/2.0), (int) graspVal);
                    
                    }
                }
            }
        }

        if (sev.type == SPNAV_EVENT_BUTTON)
        { /* SPNAV_EVENT_BUTTON */

            if (sev.button.bnum == 0 && sev.button.press)
            {
                graspVal = abs(graspVal - 30);

                printf("%d\n", graspVal);
                grasp(graspVal);

            }

            else if (sev.button.bnum == 1)
            {

                if (sev.button.press)
                    clock_gettime(CLOCK_MONOTONIC_RAW, &pressKey);
                else
                    clock_gettime(CLOCK_MONOTONIC_RAW, &releaseKey);

                //if it took at least 2 seconds to release, then end the demo
                if (timediff(releaseKey, pressKey) > 2000)
                {
                    // fprintf(training_file, "%s\n", "end");

                    printf("\n %s \n", "End of Demonstration");
                    
                    gotoPose(0, 120, 200 , pitch, -90);

                    graspVal = 0;
                    grasp(graspVal);

                    fclose(training_file);
                    break;
                }
                else
                {
                    if (sev.button.press)
                    {

                        fprintf(training_file, "%s\n", "finish");
                        fprintf(training_file_2, "%s\n", "finish");
                        printf("\n %s \n", "End of current Demonstration");

                        gotoPose(0, 120, 200 , pitch, -90);
                        graspVal = 0;
                        grasp(graspVal);

                         x = 0;
                         y = 120;
                         z = 200;
                         roll = -90;
                                            
                    }
                }
            }
        }
    }

#endif

#if TRAIN

    FILE *execution_file = fopen("applicationData/executiondata.txt", "w");
    FILE *hypotheses_file = fopen("applicationData/hypotheses.txt", "w");

    if (execution_file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

#if !DEMO

    spnav_event sev;
    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to the space navigator daemon\n");
        return 1;
    }

#endif

    bool trained = false;
    //train PSL using demonstration data

    while (spnav_wait_event(&sev))
    {
        if (sev.type == SPNAV_EVENT_BUTTON)
        {
            if (sev.button.bnum == 1)
            {
                if (sev.button.press)
                    clock_gettime(CLOCK_MONOTONIC_RAW, &pressKey);
                else
                    clock_gettime(CLOCK_MONOTONIC_RAW, &releaseKey);

                if (timediff(releaseKey, pressKey) > 2000)
                {
                    printf("\n %s \n", "Start training");

                    hyptrain();

                    printf("\n %s \n", "Training done");

                    trained = true;

                    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

                    break;
                }
            }
        }
    }

    spnav_close();

    // Event_t *events_ = new Event_t();
    EventUnion e;

    print_hypotheses(hypotheses_file);

    while (true && trained)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &counter);

        if (timediff(counter, start) > 500) //observe every 1000ms
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            // Observation: objectpose - endeffector pose

            cap >> frame;
            float *ff = getObjectPose(frame, segmentation_values, width, height );

            float *objectpose = new float[4]; //delta (x, y, z, theta)
            imagePoint.x = ff[0];
            imagePoint.y = ff[1];

            inversePerspectiveTransformation(imagePoint, camera_model, 0, &worldPoint);
            
            objectpose[0] = x - worldPoint.x;
            objectpose[1] = y -  worldPoint.y;
            objectpose[2] = z - worldPoint.z;
            objectpose[3] = roll - ff[2];

            e.observation.diffX = objectpose[0] + 0.5;
            e.observation.diffY = objectpose[1] + 0.5;
            e.observation.diffZ = objectpose[2] + 0.5;
            e.observation.diffangle = 0.0; //objectpose[3] + 0.5;

            e.observation.grasp = graspVal;

            fprintf(execution_file, "Observation: %f %f %f %f %d\n", objectpose[0], objectpose[1], objectpose[2], objectpose[3], graspVal);
            fprintf(execution_file, "Observation: %d %d %d %d %d\n", (int) e.observation.diffX, (int) e.observation.diffY, (int) e.observation.diffZ, (int) e.observation.diffangle, (int) e.observation.grasp);

            Event_t *events_ = new Event_t();
            push(events_, e, 2);

            Event_t *pred = predict(events_);
            push(events_, pred->event, pred->eventtype);

            // fprintf(execution_file, "Action: %f %f %f %f %f %d\n", (float) pred->event.action.deltaX, (float) pred->event.action.deltaY, (float) pred->event.action.deltaZ, pitch, (float) pred->event.action.deltaangle, pred->event.action.grasp);
            
            fprintf(execution_file, "Action: %d %d %d %d %d %d evtype: %d \n",  (int) pred->event.action.deltaX,  (int) pred->event.action.deltaY,  (int) pred->event.action.deltaZ, (int) pitch, (int) pred->event.action.deltaangle, (int) pred->event.action.grasp, (int) pred->eventtype);
            
            if(pred->eventtype != 0)
            {
                // int status = gotoPose(x + (float) pred->event.action.deltaX, y + (float) pred->event.action.deltaY, z + (float) pred->event.action.deltaZ, pitch, roll + (float) pred->event.action.deltaangle);
                int status = gotoPose(x + (float) pred->event.action.deltaX, y + (float) pred->event.action.deltaY, z + (float) pred->event.action.deltaZ, pitch, roll );
                
                
                if(status){

                    grasp(pred->event.action.grasp);
                    graspVal = pred->event.action.grasp;

                    x += (float) pred->event.action.deltaX;
                    y += (float) pred->event.action.deltaY;
                    z += (float) pred->event.action.deltaZ; 
                    // roll += 0.0; 
                    roll += (float) pred->event.action.deltaangle;
                }
            }
           
        }
    }

    free_hyp();
    fclose(execution_file);    

#endif

    spnav_close();
    cap.release();

    //routine for manual input

#if DEBUG

    printf("\nEnter the cartesian pose values and press enter twice...\n");
    printf("\nHit 'q' to exit...\n");

    char buffer = 0;

    x = 100;
    y = 200;
    z = 200;
    pitch = -180;
    roll = 0;

    grasp(0);

    while (buffer != 'q')
    {
        gotoPose(x, y, z, pitch, roll);
        int i = 0;
        float arr[10];
        char temp;

        do
        {
            scanf("%f%c", &arr[i], &temp);
            i++;

        } while (temp != '\n');

        for (int j = 0; j < i; j++)
        {
            switch (j)
            {
            case 0:
                x = arr[j];
                break;

            case 1:
                y = arr[j];
                break;

            case 2:
                z = arr[j];
                break;

            case 3:
                pitch = arr[j];
                break;

            case 4:
                roll = arr[j];
                break;
            };
        }

        buffer = getchar();
    }

#endif

#if CALIBRATE

    sleep(10);

    int min_z = 100;
    int max_z = 270;
    int min_y = 130;
    int max_y = 240;
    int min_x = -90;
    int max_x = 75;
    FILE *f = fopen("applicationData/calibrationdata.txt", "w");

    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    grasp(30);

    for (int x = min_x; x < max_x; x += 20)
    {
        for (int y = min_y; y < max_y; y += 20)
        {
            gotoPose(x, y, min_z, pitch, roll);

            sleep(2);

            cap >> frame;
            float *ff = getObjectPose(frame, segmentation_values, width, height );

            printf("\n %f %f %f \n", ff[0], ff[1], ff[2]);

            float i = ff[0];
            float j = ff[1];

            fprintf(f, "%d %d %d %f %f\n", x, y, min_z, i, j);

            sleep(2);
        }
    }

    fclose(f);

#endif

    delete[] segmentation_values;
    return 0;
}
