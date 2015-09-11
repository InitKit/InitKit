#ifndef UNIT___H___
#define UNIT___H___

#include <sys/wait.h>

#include "s16.h"
#include "list.h"

ListGenForNameType (pid, pid_t);

typedef enum unit_state_e
{
    S_UNINITIALISED,
    S_PRESTART,
    S_START,
    S_ONLINE,
    S_EXITED, /* for Oneshot services */
    S_STOP,
    S_STOP_TERM,
    S_STOP_KILL,
    S_MAINTENANCE,
} unit_state_e;

typedef enum unit_method_e
{
    M_PRESTART,
    M_START,
    M_STOP,
    _M_MAX,
} unit_method_e;

typedef struct unit_s
{
    svc_t * svc;
    svc_instance_t * inst;

    const char * method[_M_MAX];

    /* state-related */
    unit_state_e state;
    pid_list pids;
} unit_t;

#endif
