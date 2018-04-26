//controller.cpp

#include "controller/controllerInterface.h"
#include "app/appImplementation.h"
// #include "psl/pslImplementation.h"

struct timespec counter, start, pressKey, releaseKey;

int main()
{
    
    FILE *fp_in;
    if ((fp_in = fopen("data/ObjectTrackingInput.txt", "r")) == 0)
    {
        printf("Error can't open input file ObjectTrackingInput.txt\n");
        prompt_and_exit(1);
    }

    int * segmentation_values = new int[4];
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
    if ((fp_in2 = fopen("data/controlInput.txt", "r")) == 0)
    {
        printf("Error can't open input file controlInput.txt\n");
        prompt_and_exit(1);
    }

    // Camera Index
    int idx = CAM_IDX1;
    int idx2 = CAM_IDX2;

    // Camera Capture
    cv::VideoCapture cap;
    cv::Mat frame;

    // Camera Capture
    cv::VideoCapture cap2;
    cv::Mat frame2;

    if (!cap.open(idx))
    {
        cout << "Webcam not connected.\n"
             << "Please verify\n";
             
        prompt_and_exit(1);
    }

    if (!cap2.open(idx2))
    {
        cout << "Webcam not connected.\n"
             << "Please verify\n";

        prompt_and_exit(1);
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 768);

    cap2.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
    cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 768);


    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9";
    char BAUD[7] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    float x = 50;
    float y = 140;
    float z = 150;
    float pitch = -90;
    float roll = 0;
    int graspVal = 0; //open

    initializeControllerWithSpeed(PORT, BAUD, speed);

    goHome(5);

    gotoPose(50, 140, 150, pitch, roll);    


#if DEMO

    printf("\n %s \n", "Commence demonstration");

    FILE *training_file = fopen("data/trainingdata.txt", "w");

    if (training_file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    spnav_event sev;
    signal(SIGINT, sig);

    // spnav_sensitivity(0.1f);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to the space navigator daemon\n");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (spnav_wait_event(&sev))
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &counter);

        if (timediff(counter, start) > 200) //sample every 200 milliseconds or 5Hz
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
                    status = gotoPose(x + poseDelta[0], y, z, pitch, roll);

                else if (index == 1)
                    status = gotoPose(x, y + poseDelta[1], z, pitch, roll);

                else if (index == 2)
                    status = gotoPose(x, y, z + poseDelta[2], pitch, roll);

                else if (index == 4)
                    status = gotoPose(x, y, z, pitch, roll + poseDelta[4]);

                //printf("\n status: %d \n", status );
                if (status)
                {
                    //Action: posedeltas

                    if (index == 0)
                        x += poseDelta[0];
                    else if (index == 1)
                        y += poseDelta[1];
                    else if (index == 2)
                        z += poseDelta[2];
                    else if (index == 4)
                        roll += poseDelta[4];

                        fprintf(training_file, " %f %f %f %f %d\n", index == 0? poseDelta[0] : 0.0,  index == 1 ? poseDelta[1] : 0.0 ,  index == 2 ? poseDelta[2] : 0.0, index == 4 ? poseDelta[4] : 0.0, graspVal);

                        // fprintf(training_file, " %f %f %f %f %d\n",  poseDelta[0],  poseDelta[1],  poseDelta[2],  poseDelta[4], graspVal);

                        // Observation: objectpose - endeffector pose

                        cap >> frame;
                        float * ff = getObjectPose(frame, segmentation_values);

                        cap2 >> frame2;
                        float * ff2 = getObjectPose(frame2, segmentation_values);
                        float* objectpose = new float[4]; //delta (x, y, z, theta)
                        objectpose[0] = -1.0;
                        objectpose[1] = -1.0;
                        objectpose[2] = -1.0;
                        objectpose[3] = -1.0;
                        

                        if(ff[0] != -1)
                        {
                            if(ff[3] != -1)
                            {
                                objectpose[0] = (ff[3] - ff[0]);
                                objectpose[1] = (ff[4] - ff[1]);
                                objectpose[3] = (ff[5] - ff[2]);

                                if(ff2[0] != -1)
                                {
                                    if(ff2[3] != -1)
                                    {
                                        objectpose[2] = +(ff2[3] - ff[0]);
                                    }
                                }
                            }
                        }

                        //endeffector and object differential pose

                        printf("\n %f %f %f %f \n", objectpose[0], objectpose[1], objectpose[2], objectpose[3]);
                        fprintf(training_file, " %f %f %f %f\n", objectpose[0], objectpose[1], objectpose[2], objectpose[3]);
                        

                }
            }
        }

        if (sev.type == SPNAV_EVENT_BUTTON)
        { /* SPNAV_EVENT_BUTTON */

            if (sev.button.bnum == 0 && sev.button.press)
            {
                graspVal = abs(graspVal - 60);

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
                    fprintf(training_file, "%s\n", "end");

                    printf("\n %s \n", "End of Demonstration");

                    gotoPose(50, 140, 150, pitch, 0);    
                    
                    fclose(training_file);
                    break;
                    
                }
                else
                {
                    if(sev.button.press){

                        fprintf(training_file, "%s\n", "finish");                    
                        printf("\n %s \n", "End of current Demonstration");
                    
                    }
                    
                }
            }
        }
    }
    
#endif

#if TRAIN

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
    
    Event_t *events_ = new Event_t();
    EventUnion e;

    print_hypotheses();

    while (true && trained)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &counter);

        if (timediff(counter, start) > 200) //observe every 200ms
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            // Observation: objectpose - endeffector pose

            cap >> frame;
            float * ff = getObjectPose(frame, segmentation_values);

            cap2 >> frame2;
            float * ff2 = getObjectPose(frame2, segmentation_values);
            float* objectpose = new float[4]; //delta (x, y, z, theta)
            objectpose[0] = -1.0;
            objectpose[1] = -1.0;
            objectpose[2] = -1.0;
            objectpose[3] = -1.0;
            

            if(ff[0] != -1)
            {
                if(ff[3] != -1)
                {
                    objectpose[0] = (ff[3] - ff[0]);
                    objectpose[1] = (ff[4] - ff[1]);
                    objectpose[3] = (ff[5] - ff[2]);

                    if(ff2[0] != -1)
                    {
                        if(ff2[3] != -1)
                        {
                            objectpose[2] = +(ff2[3] - ff[0]);
                        }
                    }
                }
            }

            e.observation.diffX = objectpose[0] + 0.5;
            e.observation.diffY = objectpose[1] + 0.5;            
            e.observation.diffZ = objectpose[2] + 0.5;
            e.observation.diffangle = objectpose[3] + 0.5;

            push(events_, e, 2);

            Event_t *pred = predict(events_);
            push(events_, pred->event, pred->eventtype);

            gotoPose(pred->event.action.deltaX, pred->event.action.deltaY, pred->event.action.deltaZ, pitch, pred->event.action.deltaangle);
            grasp(pred->event.action.grasp);

        }
    }

free_hyp();

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
    pitch = -90;
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
    FILE *f = fopen("data/calibrationdata.txt", "w");

    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    grasp(40);


    for (int x = min_x; x < max_x; x += 20)
    {
        for (int y = min_y; y < max_y; y += 20)
        {
            gotoPose(x, y, min_z, pitch, roll);

            sleep(2);
            
            cap >> frame;
            float * ff = getObjectPose(frame, segmentation_values);

            printf("\n %f %f %f \n", ff[0], ff[1], ff[2]);

            float i = ff[0];
            float j = ff[1];

            fprintf(f, "%d %d %d %f %f\n", x, y, min_z, i, j);

            sleep(2);
        }
    }

    fclose(f);

#endif

    delete [] segmentation_values;
    return 0;
}
