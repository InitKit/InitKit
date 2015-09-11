#ifndef MANAGER___H___
#define MANAGER___H___

#include <threads.h>
#include <s16.h>
#include <s16rr.h>

#include "list.h"

enum msg_type_e
{
    MSG_START,
    MSG_STOP,
    MSG_RELOAD,
    MSG_RESTART,
    MSG_AVAILABLE,
    MSG,
};

typedef struct msg_s
{
    enum msg_type_e type;
    svc_id_t id, i_id;
} msg_t;

ListGenForNameType (msg, msg_t);

typedef struct manager_s
{
    msg_list mqueue;

    int kq;
    process_tracker_t * ptrack;
    thrd_t thrd_rpc;
} manager_t;

extern manager_t Manager;

#endif
