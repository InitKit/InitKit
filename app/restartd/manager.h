#ifndef MANAGER___H___
#define MANAGER___H___

#include <threads.h>
#include <s16rr.h>

typedef struct manager_s
{
    int kq;
    process_tracker_t * ptrack;
} manager_t;

extern manager_t Manager;

#endif
