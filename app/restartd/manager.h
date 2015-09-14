#ifndef MANAGER___H___
#define MANAGER___H___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "list.h"
#include "s16.h"
#include "s16rr.h"

#include "unit.h"
#include "timer.h"

typedef enum msg_type_e
{
    MSG_START,
    MSG_STOP,
    MSG_RELOAD,
    MSG_RESTART,
    MSG_AVAILABLE,
    MSG,
} msg_type_e;

typedef struct msg_s
{
    enum msg_type_e type;
    svc_id_t id, i_id;
    void * misc;
} msg_t;

ListGenForNameType (msg, msg_t);

typedef struct manager_s
{
    int kq;
    CLIENT * clnt_cfg;
    process_tracker_t * ptrack;

    thrd_t thrd_rpc;
    mtx_t lock; /* this is not really necessary, as the list is already
                   threadsafe, but it may help synchronise kevent dispatch. */
    msg_list msgs;

    unit_list units;
    timer_list timers;
} manager_t;

extern manager_t Manager;

#define NOTE_AWAKE 711
#define NOTE_IDENT 712

void note_awake ();
void note_send (enum msg_type_e type, svc_id_t id, svc_id_t i_id, void * misc);

#endif
