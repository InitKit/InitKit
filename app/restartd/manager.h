#ifndef MANAGER___H___
#define MANAGER___H___

#include <threads.h>

typedef struct Manager
{
    mtx_t lock;
    thrd_t rpc_thread;
}

#endif
