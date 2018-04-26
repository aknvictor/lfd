
#include "pslImplementation.h"
#include "fstream"
#include "cstring"

#define DEBUG 0

using namespace std;

int main(void)
{

    Event_t *events = new Event_t();

    EventUnion e;

#if DEBUG

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'c';
    e.action.deltaX = 'c';
    e.action.deltaY = 'c';
    e.action.deltaZ = 'c';
    e.action.grasp = 'c';

    push(events, e, 1);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'c';
    e.action.deltaX = 'c';
    e.action.deltaY = 'c';
    e.action.deltaZ = 'c';
    e.action.grasp = 'c';

    push(events, e, 1);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'c';
    e.action.deltaX = 'c';
    e.action.deltaY = 'c';
    e.action.deltaZ = 'c';
    e.action.grasp = 'c';

    push(events, e, 1);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'c';
    e.action.deltaX = 'c';
    e.action.deltaY = 'c';
    e.action.deltaZ = 'c';
    e.action.grasp = 'c';

    push(events, e, 1);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    Event_t *events_ = new Event_t();

    EventUnion e_;

    e_.action.deltaangle = 'a';
    e_.action.deltaX = 'a';
    e_.action.deltaY = 'a';
    e_.action.deltaZ = 'a';
    e_.action.grasp = 'a';

    push(events_, e_, 1);

#else

    FILE *fp_in;
    if ((fp_in = fopen("../data/trainingdata.txt", "r")) == 0)
    {
        printf("Error can't open input trainingdata.txt\n");
        exit(0);
    }


    ifstream inFile;
    inFile.open("../data/trainingdata.txt");

    float _da, _dx, _dy, _dz, _g, _dfx, _dfy, _dfz, _dfa;

    string line;

    bool actionread = false;

    while (getline(inFile, line))
    {
        // cout << "\n";
        if (line != "end" && line != "finish")
        {

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
                
                e.action.deltaangle = _da + 0.5;
                e.action.deltaX = _dx + 0.5;
                e.action.deltaY = _dy + 0.5;
                e.action.deltaZ = _dz + 0.5;
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

                // printf("%f %f %f %f ", _dfx, _dfy, _dfz, _dfa);
                
                e.observation.diffZ = _dfz + 0.5;
                e.observation.diffY = _dfy + 0.5;
                e.observation.diffX = _dfx + 0.5;
                e.observation.diffangle = _dfa + 0.5;

                push(events, e, 2);

            }

            free(dup);
        }
    }

    Event_t *events_ = new Event_t();
    
    e.observation.diffZ = -17.447506 + 0.5;
    e.observation.diffY = -87.288483 + 0.5;
    e.observation.diffX = -25.784897 + 0.5;
    e.observation.diffangle = -0.078953 + 0.5;

    push(events_, e, 2);


#endif

    train(events, 0, getEventSeqLen(events) - 1);

    for (int i = 0; i < 5; i++)
    {
        Event_t *pred = predict(events_);
        push(events_, pred->event, pred->eventtype);
    }

    
    print_list(events_);

    printf("\n\n");

    print_hypotheses();

    delete events_;
    delete events;

    // string _a = "cacd";
    // string _b = "bcbacbbb";

    // dynamicprogramming(_a, _b, 0);

    free_hyp();

    return 0;
}