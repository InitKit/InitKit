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
ListGenForNameType (unit, unit_t);

typedef struct manager_s
{
    int kq;
    CLIENT * clnt_cfg;
    process_tracker_t * ptrack;
    thrd_t thrd_rpc;

    unit_list units;
} manager_t;

extern manager_t Manager;

#define NOTE_IDENT 712

inline void note_send (enum msg_type_e type, svc_id_t id, svc_id_t i_id,
                       void * misc)
{
    struct kevent ev;
    msg_t * msg = malloc (sizeof (msg_t));

    msg->type = type;
    msg->id = id;
    msg->i_id = i_id;
    msg->misc = misc;

    memset (&ev, 0, sizeof (ev));
    EV_SET (&ev, NOTE_IDENT, EVFILT_USER, 0, NOTE_TRIGGER, 0, msg);

    if (kevent (Manager.kq, &ev, 1, NULL, 0, 0) == -1)
    {
        perror ("kevent! (trigger EVFILT_USER)");
    }
}

#endif
