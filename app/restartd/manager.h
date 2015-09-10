#ifndef MANAGER___H___
#define MANAGER___H___

#include <threads.h>

typedef struct manager_s
{
    mtx_t lock;
    thrd_t rpc_thread;
} manager_t;

extern manager_t Manager;

#endif
