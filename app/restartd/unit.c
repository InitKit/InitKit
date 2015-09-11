#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "manager.h"
#include "unit.h"

void unit_register_pid (unit_t * unit, pid_t pid)
{
    pid_t * rpid = malloc (sizeof (pid_t));
    *rpid = pid;
    pid_list_add (unit->pids, rpid);
}

void unit_deregister_pid (unit_t * unit, pid_t pid)
{
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        if (*it->val == pid)
        {
            pid_t * tofree = it->val;
            pid_list_del (unit->pids, it->val);
            pt_disregard_pid (Manager.ptrack, pid);
            free (tofree);
            return;
        }
    }
}

int unit_fork_and_register (unit_t * unit, const char * cmd)
{
    process_wait_t * pwait = process_fork_wait (cmd);
    if (pwait->pid == 0)
    {
        fprintf (stderr, "failed to fork for command %s\n", cmd);
        return -1;
    }
    printf ("Child PID: %d\n", pwait->pid);
    unit_register_pid (unit, pwait->pid);
    pt_watch_pid (Manager.ptrack, pwait->pid);
    process_fork_continue (pwait);
}

int unit_has_pid (unit_t * unit, pid_t pid)
{
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != 0;
         pid_list_iterator_next (&it))
    {
        if (*it->val == pid)
        {
            return 1;
        }
    }
    return 0;
}

unit_t * unit_find_by_pid (unit_list box, pid_t pid)
{
    unit_list_iterator i;
    for (i = unit_list_begin (box); i != NULL; unit_list_iterator_next (&i))
        if (unit_has_pid (i->val, pid))
            return i->val;
    return 0;
}

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
    unitnew->state = S_OFFLINE;
    unitnew->pids = List_new ();

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

void unit_ctrl (unit_t * unit, msg_type_e ctrl)
{
    printf ("Ctrl: %d\n", ctrl);
    switch (ctrl)
    {
    case MSG_START:
        if (unit->state != S_OFFLINE && unit->state != S_MAINTENANCE)
            return;
        {
            unit_fork_and_register (unit, "/bin/sh");
        }
    }
}

void unit_ptevent (unit_t * unit, pt_info_t * info)
{
    if (info->event == PT_CHILD)
        unit_register_pid (unit, info->pid);
    else if (info->event == PT_EXIT)
        unit_deregister_pid (unit, info->pid);
}
