#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "manager.h"
#include "unit.h"

void unit_timer_event (void * data, long id);

#define UnitTimerReg()                                                         \
    unit->timer_id = timer_add (unit->timeout_secs, unit, unit_timer_event);

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

pid_t unit_fork_and_register (unit_t * unit, const char * cmd)
{
    process_wait_t * pwait = process_fork_wait (cmd);
    pid_t ret;

    if (pwait == NULL || pwait->pid == 0)
    {
        fprintf (stderr, "failed to fork for command %s\n", cmd);
        return ret;
    }

    ret = pwait->pid;
    printf ("Child PID: %d\n", ret);
    unit_register_pid (unit, ret);
    pt_watch_pid (Manager.ptrack, ret);
    process_fork_continue (pwait);

    return ret;
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

    unitnew->name = inst_object_get_property_string (inst, "S16.FMRI");
    unitnew->svc = svc;
    unitnew->inst = inst;
    unitnew->timer_id = 0;
    unitnew->main_pid = 0;
    unitnew->rtype = R_YES;
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
        fprintf (stderr, "Unit <%s> lacks a known type\n", unitnew->name);
        return 0;
    }

    unitnew->method[M_PRESTART] =
        svc_object_get_property_string (svc, "Method.Prestart");
    unitnew->method[M_START] =
        svc_object_get_property_string (svc, "Method.Start");
    unitnew->method[M_STOP] =
        svc_object_get_property_string (svc, "Method.Stop");

    unitnew->timeout_secs = 2;

    return unitnew;
}

void unit_enter_stop (unit_t * unit);

void unit_enter_offline (unit_t * unit)
{
    printf ("Unit %s entering offline state\n", unit->name);
    unit->state = S_OFFLINE;
}

void unit_enter_maintenance (unit_t * unit)
{
    printf ("Unit %s entering maintenance state\n", unit->name);
    unit->target = S_MAINTENANCE;
    if (List_count (unit->pids))
        unit_enter_stop (unit);
    else
    {
        printf ("Unit %s arrives in maintenance\n", unit->name);
        unit->state = S_MAINTENANCE;
    }
}

void unit_enter_stopkill (unit_t * unit)
{
    unit->state = S_STOP_KILL;
    UnitTimerReg ();
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        kill (*it->val, SIGKILL);
    }
}

void unit_enter_stopterm (unit_t * unit)
{
    unit->state = S_STOP_TERM;
    UnitTimerReg ();
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        kill (*it->val, SIGTERM);
    }
}

void unit_enter_stop (unit_t * unit)
{
    if (unit->method[M_STOP])
    {
        unit->state = S_STOP;
        UnitTimerReg ();
        unit->main_pid = unit_fork_and_register (unit, unit->method[M_STOP]);
        if (!unit->main_pid)
        {
            unit_enter_maintenance (unit);
            return;
        }
    }
    else
        unit_enter_stopterm (unit);
}

void unit_enter_prestart (unit_t * unit)
{
    if (unit->method[M_PRESTART])
    {
        unit->state = S_PRESTART;
        UnitTimerReg ();
        unit->main_pid =
            unit_fork_and_register (unit, unit->method[M_PRESTART]);
        if (!unit->main_pid)
        {
            unit_enter_maintenance (unit);
            return;
        }
    }
}

void unit_enter_start (unit_t * unit)
{
    /* deal with forking case later */
    unit->state = S_START;
    unit->main_pid = unit_fork_and_register (unit, unit->method[M_PRESTART]);
    if (!unit->main_pid)
    {
        unit_enter_maintenance (unit);
        return;
    }
}

void unit_enter_state (unit_t * unit, unit_state_e state)
{
    switch (state)
    {
    case S_OFFLINE:
        unit_enter_offline (unit);
        break;
    case S_MAINTENANCE:
        unit_enter_maintenance (unit);
        break;
    case S_PRESTART:
        unit_enter_prestart (unit);
        break;
    case S_START:
        unit_enter_start (unit);
        break;
    }
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
            unit_enter_prestart (unit);
        }
    }
}

void unit_timer_event (void * data, long id)
{
    unit_t * unit = data;
    unit->timer_id = 0;
    timer_del (id);

    switch (unit->state)
    {
    case S_STOP_TERM:
    {
        printf ("timeout in stopterm\n");
        unit_enter_stopkill (unit);
        return;
    }
    case S_STOP_KILL:
    {
        printf ("timeout in stopkill(!)\n");
        unit_enter_state (unit, unit->target);
        return;
    }
    case S_PRESTART:
    {
        printf ("timeout in prestart\n");
        if (unit->rtype == R_YES)
        {
            printf ("restarting for prestart\n");
            unit_enter_prestart (unit);
        }
        else
            unit_enter_maintenance (unit);
        return;
    }
    }
}

void unit_ptevent (unit_t * unit, pt_info_t * info)
{
    if (info->event == PT_CHILD)
        unit_register_pid (unit, info->pid);
    else if (info->event == PT_EXIT)
        unit_deregister_pid (unit, info->pid);

    if (info->event == PT_EXIT && info->pid == unit->main_pid)
    {
        /* if exit was S16_FATAL, go to maintenance instead */
        if (exit_was_abnormal (info->flags))
        {
            printf ("Bad exit in a main pid\n");
            if (unit->timer_id)
                timer_del (unit->timer_id);
            if (unit->rtype == R_NO)
                unit->target = S_OFFLINE;
            else
                unit->target = unit->state;

            if (List_count (unit->pids))
                unit_enter_stop (unit);
            else
                unit_enter_state (unit, unit->target);
        }
    }
    switch (unit->state)
    {
    case S_STOP:
    case S_STOP_TERM:
    case S_STOP_KILL:
        if (!List_count (unit->pids))
            timer_del (unit->timer_id);
        unit->main_pid = 0;
        unit->timer_id = 0;
        printf ("All PIDs purged\n");

        break;
    }
}
