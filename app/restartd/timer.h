#ifndef TIMER___H___
#define TIMER___H___

#include "list.h"

typedef struct Timer Timer;

ListGenForNameType (timer, Timer);

typedef struct Timer
{
    long id;
    void * userData;
    void (*cb) (void *, long);
} Timer;

Timer * timer_find (long id);
void timer_del (long id);
long timer_add (int sec, void * userData, void (*cb) (void *, long));

#endif
