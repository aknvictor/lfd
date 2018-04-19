
#include "pslImplementation.h"
// #include "approxmatch.h"

#define DEBUG 1
int main(void)
{

    Event_t * events = new Event_t();

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
    
    Event_t * events_ = new Event_t();

    EventUnion e_;    

    e_.action.deltaangle = 'a';
    e_.action.deltaX = 'a';
    e_.action.deltaY = 'a';
    e_.action.deltaZ = 'a';
    e_.action.grasp = 'a';

    push(events_, e_, 1);

#else

    FILE *fp_in;
   
    if ((fp_in = fopen("../data/trainingdata.txt","r")) == 0) {
        printf("Error can't open input trainingdata.txt\n");
        exit(0);
    }

   int end_of_file;
   float _da, _dx, _dy, _dz, _g, _dfx, _dfy, _dfz, _dfa;  

    do{

        end_of_file = fscanf(fp_in, "%f %f %f %f %f ", &_dx, &_dy, &_dz, &_da, &_g);

        end_of_file = fscanf(fp_in, "%f %f %f %f ", &_dfx, &_dfy, &_dfz, &_dfa);
        
        e.action.deltaangle = _da;
        e.action.deltaX = _dx;
        e.action.deltaY = _dy;
        e.action.deltaZ = _dz;
        e.action.grasp = _g;

        
        push(events, e, 1);

        e.observation.diffZ = _dfz;
        e.observation.diffY = _dfy;
        e.observation.diffX = _dfx;
        e.observation.diffangle = _dfa;
    
        push(events, e, 2);
        
    }

    while (end_of_file != EOF);

    Event_t * events_ = new Event_t();

    EventUnion e_;

    e_.observation.diffZ = 42.492157;
    e_.observation.diffY = -80.858421;    
    e_.observation.diffX = 2.681091;
    e_.observation.diffangle = 0.336754 ;

    push(events_, e_, 2);

#endif
    
    train(events, 0, getEventSeqLen(events) - 1);

    delete events;

    for(int i = 0; i< 5; i++)
    {
        Event_t * pred =  predict(events_);
        push(events_, pred->event, pred->eventtype);
    }

    printf("\n\n");
    print_list(events_);
    print_hypotheses();

    delete events_;

    // string _a = "cacd";
    // string _b = "bcbacbbb"; 

    // dynamicprogramming(_a, _b, 0);

    free_hyp();

    return 0;

}