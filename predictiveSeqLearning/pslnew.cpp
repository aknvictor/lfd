
#include "pslImplementation.h"
#include "fstream"
#include "cstring"

#define DEBUG 0

using namespace std;

int main(void)
{

    Event_t *events = new Event_t();
    init();
    EventUnion e;

#if DEBUG

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 2);

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

    push(events, e, 2);

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

    push(events, e, 2);

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

    push(events, e, 2);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    train(events, 0, getEventSeqLen(events) - 1);
    events = new Event_t();

    e.action.deltaangle = 'a';
    e.action.deltaX = 'a';
    e.action.deltaY = 'a';
    e.action.deltaZ = 'a';
    e.action.grasp = 'a';

    push(events, e, 2);

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

    push(events, e, 2);

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

    push(events, e, 2);

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

    push(events, e, 2);

    e.observation.diffZ = 'c';
    e.observation.diffY = 'c';
    e.observation.diffX = 'c';
    e.observation.diffangle = 'c';

    push(events, e, 2);

    train(events, 0, getEventSeqLen(events) - 1);

    Event_t *events_ = new Event_t();

    EventUnion e_;

    e_.action.deltaangle = 'a';
    e_.action.deltaX = 'a';
    e_.action.deltaY = 'a';
    e_.action.deltaZ = 'a';
    e_.action.grasp = 'a';

    push(events_, e_, 1);

#else
    init();

    ifstream inFile;
    inFile.open("../applicationData/trainingdata.txt");

    int _da, _dx, _dy, _dz, _g, _dfx, _dfy, _dfz, _dfa, _dfg;

    string line;

    bool actionread = false;

    while (getline(inFile, line))
    {
        if (line != "end")
        {
            if(line == "finish")
            {
                train(events, 0, getEventSeqLen(events) - 1);
                events = new Event_t();

                continue;
            }

            if (!actionread)
            {            
                //read action
                sscanf(line.c_str(), " %d %d %d %d %d", &_dx, &_dy, &_dz, &_da, &_g);                                

                actionread = true;

                printf("%d %d %d %d %d ", _dx, _dy, _dz, _da, _g);
                
                e.action.deltaangle = _da + 0.5;
                e.action.deltaX = _dx + 0.5;
                e.action.deltaY = _dy + 0.5;
                e.action.deltaZ = _dz + 0.5;
                e.action.grasp = _g;

                push(events, e, 2);

            }
            else
            {
                //read obseration

                actionread = false;

                sscanf(line.c_str(), " %d %d %d %d %d", &_dfx, &_dfy, &_dfz, &_dfa, &_dfg);                               
                
                printf("%d %d %d %d ", _dfx, _dfy, _dfz, _dfa);
                
                e.observation.diffZ = _dfz + 0.5;
                e.observation.diffY = _dfy + 0.5;
                e.observation.diffX = _dfx + 0.5;
                e.observation.diffangle = _dfa + 0.5;
                e.observation.grasp = _dfg + 0.5;

                push(events, e, 2);

            }
        }
    }

    Event_t *events_ = new Event_t();
       
    e.observation.diffZ = 42.432213 + 0.5;
    e.observation.diffY = -57.938049 + 0.5;
    e.observation.diffX = -8.272209 + 0.5;
    e.observation.diffangle = -9.737320 + 0.5;

    push(events_, e, 2);


#endif

    // train(events, 0, getEventSeqLen(events) - 1);

    print_hypotheses();

    for (int i = 0; i < 5; i++)
    {
        Event_t *pred = predict(events_);
        push(events_, pred->event, pred->eventtype);
    }

    
    print_list(events_);

    printf("\n\n");


    delete events_;
    delete events;

    string _a = "-1-7-10";
    string _b = "-1-5-10";

    // printf("\n\n dist-  %d", levDistance(_a, _b));

    free_hyp();

    return 0;
}