#ifndef UNIT___H___
#define UNIT___H___

#include <sys/wait.h>

#include "s16.h"
#include "list.h"

typedef struct unit_s unit_t;

ListGenForNameType (pid, pid_t);
ListGenForNameType (unit, unit_t);

typedef enum unit_type_e
{
    T_EXEC,
    T_FORKS,
    T_ONESHOT,
} unit_type_e;

typedef enum unit_state_e
{
    S_OFFLINE,
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
    unit_type_e type;

    /* state-related */
    unit_state_e state;
    pid_list pids;
    pid_t main_pid;

    unsigned int timer_id;
} unit_t;

int unit_has_pid (unit_t * unit, pid_t pid);

unit_t * unit_find (unit_list box, svc_id_t id, svc_id_t id_i);
unit_t * unit_new (svc_t * svc, svc_instance_t * inst);
typedef enum msg_type_e msg_type_e;
void unit_ctrl (unit_t * unit, msg_type_e ctrl);

#endif
