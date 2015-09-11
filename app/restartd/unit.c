#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "manager.h"
#include "unit.h"

unit_t * unit_find (unit_list box, svc_id_t id, svc_id_t id_i)
{
    unit_list_iterator i;
    for (i = unit_list_begin (box); i != NULL; unit_list_iterator_next (&i))
        if (i->val->svc->id == id && i->val->inst->id == id_i)
            return i->val;
    return 0;
}

unit_t * unit_new (svc_t * svc, svc_instance_t * inst)
{
#define CompareType(typ)                                                       \
    !strcasecmp (svc_object_get_property_string (svc, "Unit.Form"), typ)

    unit_t * unitnew = malloc (sizeof (unit_t));

    unitnew->svc = svc;
    unitnew->inst = inst;
    unitnew->timer_id = 0;
    unitnew->main_pid = 0;
    unitnew->state =S_OFFLINE;
    unitnew->pids = List_new();

    if (CompareType ("exec"))
        unitnew->type = T_EXEC;
    else if (CompareType ("forks"))
        unitnew->type = T_FORKS;
    else if (CompareType ("oneshot"))
        unitnew->type = T_ONESHOT;
    else
    {
        fprintf (stderr, "Unit <%s> lacks a known type\n", svc->name);
        return 0;
    }

    unitnew->method[M_PRESTART] =
        svc_object_get_property_string (svc, "Method.Prestart");
    unitnew->method[M_START] =
        svc_object_get_property_string (svc, "Method.Start");
    unitnew->method[M_STOP] =
        svc_object_get_property_string (svc, "Method.Stop");

    return unitnew;
}

void unit_ctrl(unit_t * unit, msg_type_e ctrl)
{
    printf("Ctrl: %d\n", ctrl);
    switch (ctrl)
    {
    case MSG_START:
        if(unit->state != S_OFFLINE && unit->state != S_MAINTENANCE) return;
        {
            process_wait_t * pwait = process_fork_wait("/bin/sh");
            printf("Child PID: %d\n", pwait->pid);
            process_fork_continue(pwait);
        }
    }
}
