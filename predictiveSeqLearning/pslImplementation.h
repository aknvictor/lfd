#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "approxmatch.h"


#define EVENTLEN 270
#define MAXEVENT 20000
#define MAXHYP 10000
#define VTFACTOR 2.0

// #define NULL ((void *)0)

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

using namespace std;

//lhs represents the hypothesis body and rhs represents the head in PSL nomenclature
//misses and hits are used to calculate the support and confidence of an hypothesis
//id = 0 means the hypothesis is NULL (or empty). as C doesn't support assigning NULL to an hypothesis.

typedef struct
{
    signed char diffX;
    signed char diffY;
    signed char diffZ;
    signed char diffangle;
    signed char grasp;    

} Observation;

typedef struct
{
    signed char deltaX;
    signed char deltaY;
    signed char deltaZ;
    signed char deltaangle;
    signed char grasp;

} Action;

typedef union {
    Observation observation;
    Action action;

} EventUnion;

//eventtype 1 - Action, 2 - Observation, 0 - notdefined

typedef struct Event
{
    EventUnion event;
    int eventtype;
    struct Event *next;

} Event_t;

typedef struct
{

    int id;
    Event_t *lhs;
    Event_t *rhs;
    int misses;
    int hits;

} Hypothesis;


void init();

Hypothesis newHyp();

void print_list(Event_t *head);

void push(Event_t * head, EventUnion val, int eventtype);

int getEventSeqLen(Event_t *sequence);

Event_t * get_by_index(Event_t *head, int n);

Event_t * subsequence(Event_t *sequence, int startIndex, int stopIndex);

int getVtFactor(Hypothesis hyp);

Hypothesis grow(Event_t *sequence, Hypothesis parent);

Hypothesis grow_sub(Event_t *sequence, Event_t *parent);

double conf(int hypIndex);

int support(int hypIndex);

void reward(int hypIndex, int value);

void punish(int hypIndex, int value);

int hyp_approxmatch(Event_t *a, Event_t *b);

double hypMatch(int hypIndex, Event_t *sequence);

Hypothesis selectHyp(Event_t *seq);

void getConfScores(Event_t *sequence, double hs[]);

void clearHypotheses();

int eventcompare(Event_t *a, Event_t *b);

void train(Event_t *sequence, int startIndex, int stopIndex);

Event_t *predict(Event_t *seq);

void free_hyp();

void free_event_seq(Event_t * head);

void print_hypotheses();

void print_hypotheses(FILE * outFile);
