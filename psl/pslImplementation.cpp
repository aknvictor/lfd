#include "pslImplementation.h"

int hypothesisCount = 0;
Hypothesis hypotheses[MAXHYP];

void init()
{
    //initialize the hypotheses library.
    //instantiate MAXHYP hypotheses and allocate memory for the body and head of each.

    for (int i = 0; i < MAXHYP; i++)
    {
        hypotheses[i].hits = 1;
        hypotheses[i].id = -1;

        hypotheses[i].lhs = new Event_t();
        hypotheses[i].rhs = new Event_t();

        hypotheses[i].lhs->eventtype = 0;
        hypotheses[i].lhs->next = NULL;

        hypotheses[i].rhs->eventtype = 0;
        hypotheses[i].rhs->next = NULL;
    }
}

void free_hyp()
{
    for (int i = 0; i < MAXHYP; i++)
    {
        free_event_seq(hypotheses[i].lhs);
        free_event_seq(hypotheses[i].rhs);
    }
}


Hypothesis newHyp()
{
    Hypothesis hyp;

    hyp.id = -1;
    hyp.hits = 1;
    hyp.misses = 0;
    hyp.lhs = new Event_t();
    hyp.rhs = new Event_t();

    return hyp;
}

void print_list(Event_t *head)
{

    Event_t *current = head;

    while (current != NULL)
    {

        if (current->eventtype == 1)
            printf("%d %d %d %d %d \n", current->event.action.deltaX, current->event.action.deltaY, current->event.action.deltaZ, current->event.action.deltaangle, current->event.action.grasp );
        else
            printf("%d %d %d %d \n", current->event.observation.diffX, current->event.observation.diffY, current->event.observation.diffZ, current->event.observation.diffangle); 

        current = current->next;
    }
}

void push(Event_t * head, EventUnion val, int eventtype)
{
    Event_t *current = head;
    
    if(current->next == NULL && current->eventtype == 0)
    {
        current->eventtype = eventtype;
        current->event = val;
        return;
    }

    while (current->next != NULL)
    {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = new Event_t();
    current->next->event = val;
    current->next->eventtype = eventtype;
    current->next->next = NULL;
}

int getEventSeqLen(Event_t *sequence)
{

    Event_t *head = sequence;
    int ct = 0;

    while (head != NULL)
    {
        head = head->next;
        ct++;
    }

    return ct;
}

void free_event_seq(Event_t * head)
{
    Event_t * next = NULL;
    Event_t * p = NULL;

    for(p = head; NULL != p; p = next) 
    {
        next = p->next;
        delete p;
    }
}

Event_t * get_by_index(Event_t *head, int n)
{
    int i = 0;

    Event_t *current = head;
    if (n == 0)
    {
        return head;
    }

    for (i = 0; i < n; i++)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        current = current->next;
    }

    return current;
}

Event_t * subsequence(Event_t *sequence, int startIndex, int stopIndex)
{
    Event_t * sub = new Event_t();

    if ((stopIndex > startIndex) && (getEventSeqLen(sequence) >= stopIndex))
    {
        int sz = stopIndex - startIndex ;
        Event_t * current = get_by_index(sequence, startIndex);

        for (int i = 0; i < sz; i++)
        {   
            push(sub, current->event, current->eventtype);
            current = current->next;
        }
    }

    return sub;
}

int getVtFactor(Hypothesis hyp)
{
    int l = getEventSeqLen(hyp.lhs);
    int f = floor(l * VTFACTOR);

    return max(l + 1, f);
}

Hypothesis grow(Event_t *sequence, Hypothesis parent)
{
    int slen = getEventSeqLen(sequence);
    int vtFactor;
    if(getVtFactor(parent) > slen )
         vtFactor = slen;
    else 
         vtFactor = getVtFactor(parent);

    hypotheses[hypothesisCount].lhs = subsequence(sequence, slen - vtFactor, slen);

    hypotheses[hypothesisCount].rhs = parent.rhs;

    // print_list(hypotheses[hypothesisCount].lhs);
    // printf("->");
    // print_list(hypotheses[hypothesisCount].rhs);

    hypotheses[hypothesisCount].id = hypothesisCount;

    hypothesisCount += 1;
    return hypotheses[hypothesisCount - 1];
}

Hypothesis grow_sub(Event_t *sequence, Event_t *parent)
{
    //if PSL failed to predict

    int slen = getEventSeqLen(sequence);

    Hypothesis h = newHyp();

    h.lhs = subsequence(sequence, slen - 1, slen); //add the last event sequence
    h.rhs = parent;

    // print_list(h.lhs);
    // printf("->");
    // print_list(h.rhs);

    hypotheses[hypothesisCount] = h;
    hypotheses[hypothesisCount].id = hypothesisCount;

    hypothesisCount += 1;
    return hypotheses[hypothesisCount - 1];
}

double conf(int hypIndex)
{
    double conf = (double) (getEventSeqLen(hypotheses[hypIndex].lhs) * hypotheses[hypIndex].hits) / (double)(hypotheses[hypIndex].hits + hypotheses[hypIndex].misses);
    
    // printf("%d %f \n", hypIndex, conf);

    return conf;
}

int support(int hypIndex)
{
    return hypotheses[hypIndex].misses + hypotheses[hypIndex].hits;
}

void reward(int hypIndex, int value)
{
    if (value)
    {
        hypotheses[hypIndex].hits += value;
    }
}

void punish(int hypIndex, int value)
{
    //punish the hypothesis by increasing the value of its misses

    if (value)
    {
        hypotheses[hypIndex].misses += value;
    }
}

int hyp_approxmatch(Event_t *a, Event_t *b)
{

    string _a, _b;

    Event_t * current = a;

    while (current != NULL)
    {
        if (current->eventtype == 1)
        {
            
            _a.push_back(current->event.action.deltaX);
            _a.push_back(current->event.action.deltaY);
            _a.push_back(current->event.action.deltaZ);
            _a.push_back(current->event.action.deltaangle);
            _a.push_back(current->event.action.grasp);

            // _a.push_back('1');
            

        }
        else if (current->eventtype == 2)
        {
            _a.push_back(current->event.observation.diffX);
            _a.push_back(current->event.observation.diffY);
            _a.push_back(current->event.observation.diffZ);
            _a.push_back(current->event.observation.diffangle);
            // _a.push_back('2');
            
        }

        current = current->next;
    }

    current = b;

    while (current != NULL)
    {
        if (current->eventtype == 1)
        {
            _b.push_back(current->event.action.deltaX);
            _b.push_back(current->event.action.deltaY);
            _b.push_back(current->event.action.deltaZ);
            _b.push_back(current->event.action.deltaangle);
            _b.push_back(current->event.action.grasp);

            // _b.push_back('1');
            
        }
        else if (current->eventtype == 2)
        {
            _b.push_back(current->event.observation.diffX);
            _b.push_back(current->event.observation.diffY);
            _b.push_back(current->event.observation.diffZ);
            _b.push_back(current->event.observation.diffangle);
            // _b.push_back('2');          

        }

        current = current->next;
    }

    int match = approxmatch(_a, _b, 0, 0);

    free_event_seq(current);    

    return match;
    
}

double hypMatch(int hypIndex, Event_t *sequence)
{
    //match the lhs of the hypothesis with a given sequence returning a certain confidence score
    //adjusted for the length of the both sequences

    int seqlen = getEventSeqLen(sequence);
    int hypLhsLen = getEventSeqLen(hypotheses[hypIndex].lhs);

    if (sequence)
    {
        int diff = hypLhsLen - seqlen;

        if(hypLhsLen == 0)
            return conf(hypIndex);
        
        else if(seqlen < hypLhsLen)
        {

                Event_t * sub = subsequence(hypotheses[hypIndex].lhs, diff, hypLhsLen );
                if(hyp_approxmatch(sub , sequence) == 0){

                    double z = conf(hypIndex);
                    z = z / hypLhsLen * seqlen;

                    free_event_seq(sub);
                    return z;
                }
                
        }
        
        else {

             Event_t * sub = subsequence(sequence, seqlen - hypLhsLen, seqlen );

             if(hyp_approxmatch(sub, hypotheses[hypIndex].lhs) == 0)
             {
                return conf(hypIndex);
            }
            else {            
                return 0.0;
             }

            free_event_seq(sub); //suspect 
           
        }

    }

    else
        return 0.0;

    return 0.0;
}

Hypothesis selectHyp(Event_t *seq)
{
    double * scores = new double[hypothesisCount];
    
    for (int i = 0; i < hypothesisCount; i++)
    {
        double score = hypMatch(i, seq);
        scores[i] = score;
    }
    int max = 0;
    int index = 0;
    for (int i = 0; i < hypothesisCount; i++)
    {
        if (max < scores[i])
        {
            max = scores[i];
            index = i;
        }
    }
    Hypothesis nullHyp = newHyp();

    // if(scores[index] > 0.0) printf("| %d | \n", index);

    return scores[index] > 0.0 ? hypotheses[index] : nullHyp;
}

void getConfScores(Event_t *sequence, double hs[])
{    
    if (hypothesisCount == 0)
        return;

    for (int i = 0; i < hypothesisCount; i++)
    {
        double a = hypMatch(i, sequence);
        hs[i] = a;
    }

}

void clearHypotheses()
{
    for (size_t i = 0; i < MAXHYP; i++)
    {
        hypotheses[i].id = -1;

        for (size_t j = 0; j < (size_t) getEventSeqLen(hypotheses[i].lhs); j++)
        {
            hypotheses[i].lhs[j].eventtype = 0;
        }

        hypotheses[i].rhs->eventtype = 0;
        hypotheses[i].misses = 0;
        hypotheses[i].hits = 1;
    }
}

int eventcompare(Event_t *a, Event_t *b)
{
    if (a->eventtype != b->eventtype)
        return -1;
    if (a->eventtype == 1)
    {
        if (a->event.action.deltaangle == b->event.action.deltaangle &&
            a->event.action.deltaX == b->event.action.deltaX && a->event.action.deltaY == b->event.action.deltaY &&
            a->event.action.deltaZ == b->event.action.deltaZ)
            return 0;
        else
            return -1;
    }
    else
    {
        if (a->event.observation.diffangle == b->event.observation.diffangle &&
            a->event.observation.diffX == b->event.observation.diffX && a->event.observation.diffY == b->event.observation.diffY &&
            a->event.observation.diffZ == b->event.observation.diffZ)
            return 0;
        else
            return -1;
    }
}

void train(Event_t *sequence, int startIndex, int stopIndex)
{
    init();
    int seqlen = getEventSeqLen(sequence);

    if(seqlen < 2) return;

    if (seqlen <= stopIndex || startIndex >= seqlen || startIndex > stopIndex)
        return;

    if(startIndex == 0) startIndex = 1;

    for (int i = startIndex; i < stopIndex + 1; i++)
    {
        Event_t * sub = subsequence(sequence, 0, i);
        Event_t * t = subsequence(sequence, i, i+1);

        // printf("%s", "sub: \n");

        // print_list(sub);

        double * hs = new double[hypothesisCount];

        // printf("%s", "scores (hyps => sub): ");

        getConfScores(sub, hs); //gets a confidence score for every hypothesis

        for (int j = 0; j < hypothesisCount; j++)
        {
            // printf("hs[%d]: %f ", j, hs[j]);
        }
        // printf("\n");

        Hypothesis maxh = newHyp();
        
        double maxc = -1.0;
        bool correct = false;
        Hypothesis bestCorrect = newHyp();
 
        for (int j = 0; j < hypothesisCount; j++)
        {
            double conf = hs[j];

            // printf("conf: %f\n", conf);

            if (conf > 0.0)
            {
                Hypothesis hyp = hypotheses[j];

                if (maxc < conf)
                {
                    maxc = conf;
                    maxh = hyp;
                }

                if (eventcompare(hyp.rhs, t) == 0)
                {
                    reward(j, 1);
                    // printf("reward(%d)\n" ,j+1);

                    if (bestCorrect.id == -1 || getEventSeqLen(bestCorrect.lhs) < getEventSeqLen(hyp.lhs))
                    {
                        bestCorrect = hyp;
                    }
                }
            }
        }

        //if the rhs sugessted doesn't match the next event in the sequence, punish the hypothesis
        
        correct = (maxh.id != -1) && (eventcompare(maxh.rhs, t) == 0);

        if(maxh.id != -1 && !correct)
        {
            punish(maxh.id, 1);
            // printf("punish(%d)\n", maxh.id+1);
        }

        //psl learns only on failure
        //hypothesis library is updated in grow
        
        if(!correct)
        {
            if (bestCorrect.id == -1)
            grow_sub(sub, t);
            else
            grow(sub, bestCorrect);            
        }

        free_event_seq(sub);       
    }

    // print_hypotheses();

}

Event_t *predict(Event_t *seq)
{
    Hypothesis h = selectHyp(seq);
    return h.rhs;
}


void print_hypotheses()
{
    for(int i=0; i< MAXHYP; i++)
    {
        Hypothesis hyp = hypotheses[i];
        if(hyp.id == -1) break;

        print_list(hyp.lhs);

        printf("%s\n", "->");

        print_list(hyp.rhs);

        printf("%s\n", ":");
        
    }
}