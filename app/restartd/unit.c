#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "manager.h"
#include "unit.h"

void unit_timer_event (void * data, long id);
void unit_enter_state (unit_t * unit, unit_state_e state);

#define UnitTimerReg()                                                         \
    unit->timer_id = timer_add (unit->timeout_secs, unit, unit_timer_event);

#define DbgEnteringState(x)                                                    \
    fprintf (stderr, "[%s] Unit entering state %s\n", unit->name, #x);
#define DbgEnteredState(x)                                                     \
    fprintf (stderr, "[%s] Unit arrived at state %s\n", unit->name, #x);

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
    pid_t ret = 0;

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
    !strcasecmp (svc_object_get_property_string (svc, "Unit.Strategy"), typ)

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
        List_destroy (unitnew->pids);
        free (unitnew);
        return 0;
    }

    unitnew->method[M_PRESTART] =
        svc_object_get_property_string (svc, "Method.Prestart");
    unitnew->method[M_START] =
        svc_object_get_property_string (svc, "Method.Start");
    unitnew->method[M_POSTSTART] =
        svc_object_get_property_string (svc, "Method.Poststart");
    unitnew->method[M_STOP] =
        svc_object_get_property_string (svc, "Method.Stop");

    unitnew->timeout_secs = 12;

    return unitnew;
}

void unit_enter_stop (unit_t * unit);
void unit_enter_stopterm (unit_t * unit);

void unit_enter_offline (unit_t * unit)
{
    DbgEnteredState (Offline);
    unit->state = S_OFFLINE;
}

void unit_enter_maintenance (unit_t * unit)
{
    DbgEnteringState (Maintenance);
    unit->target = S_MAINTENANCE;
    if (List_count (unit->pids))
        unit_enter_stop (unit);
    else
    {
        DbgEnteredState (Maintenance);
        unit->state = S_MAINTENANCE;
    }
}

void unit_purge_and_target (unit_t * unit)
{
    if (List_count (unit->pids))
        unit_enter_stopterm (unit);
    else
        unit_enter_state (unit, unit->target);
}

void unit_enter_stopkill (unit_t * unit)
{
    DbgEnteredState (Stopkill);
    unit->state = S_STOP_KILL;
    /* set a timer so that, should this fail to terminate all PIDs, we can do
     * something else (this should never happen with SIGKILL) */
    UnitTimerReg ();
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        kill (*it->val, SIGKILL);
    }
}

void unit_enter_stopterm (unit_t * unit)
{
    if (!List_count (unit->pids))
    {
        unit_enter_state (unit, unit->target);
        return;
    }

    DbgEnteredState (Stopterm);
    unit->state = S_STOP_TERM;
    /* first, just try to kill the main PID */
    if (unit->main_pid)
        kill (unit->main_pid, SIGTERM);
    UnitTimerReg ();
    /* now the rest */
    for (pid_list_iterator it = pid_list_begin (unit->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        kill (*it->val, SIGTERM);
    }
    note_awake (); /* prepare for more events */
}

void unit_enter_stop (unit_t * unit)
{
    DbgEnteringState (Stop);
    /* if the unit has a `stop` method, try that */
    if (unit->method[M_STOP])
    {
        DbgEnteredState (Stop);
        unit->state = S_STOP;
        UnitTimerReg ();
        unit->main_pid = unit_fork_and_register (unit, unit->method[M_STOP]);
        /* if no pid emerged, fork failed; immediately go to maintenance after
         * clearing any remaining processes */
        if (!unit->main_pid)
        {
            unit->target = S_MAINTENANCE;
            unit_purge_and_target (unit);
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
        DbgEnteredState (Prestart);
        unit->state = S_PRESTART;
        UnitTimerReg ();
        unit->main_pid =
            unit_fork_and_register (unit, unit->method[M_PRESTART]);
        if (!unit->main_pid)
        {
            unit->target = S_MAINTENANCE;
            unit_purge_and_target (unit);
            return;
        }
    }
}

void unit_enter_poststart (unit_t * unit)
{
    if (unit->method[M_POSTSTART])
    {
        DbgEnteredState (Poststart);
        unit->state = S_POSTSTART;
        UnitTimerReg ();
        unit->secondary_pid =
            unit_fork_and_register (unit, unit->method[M_POSTSTART]);
        if (!unit->secondary_pid)
        {
            unit->target = S_MAINTENANCE;
            unit_purge_and_target (unit);
            return;
        }
    }
}

void unit_enter_start (unit_t * unit)
{
    /* deal with forking case later */
    if (unit->type == T_EXEC)
    {
        DbgEnteredState (Start);

        unit->state = S_START;
        unit->main_pid = unit_fork_and_register (unit, unit->method[M_START]);
        if (!unit->main_pid)
        {
            unit_enter_maintenance (unit);
            return;
        }
        /* in a T_EXEC service, we consider it online as soon as the process
         * specified in the start method is running. */
        unit_enter_poststart (unit);
    }
}

void unit_enter_online (unit_t * unit)
{
    DbgEnteredState (Online);
    unit->state = S_ONLINE;
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
    case S_POSTSTART:
        unit_enter_poststart (unit);
        break;
    case S_ONLINE:
        unit_enter_online (unit);
        break;
    }
}

void unit_ctrl (unit_t * unit, msg_type_e ctrl)
{
    fprintf (stderr, "ctrl: %d\n", ctrl);
    switch (ctrl)
    {
    case MSG_START:
        /* you can't start a service that has already started */
        if (unit->state != S_OFFLINE && unit->state != S_MAINTENANCE)
            return;

        unit_enter_prestart (unit);
        break;
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
        fprintf (stderr, "timeout in stopterm\n");
        unit_enter_stopkill (unit);
        return;
    }
    case S_STOP_KILL:
    {
        fprintf (stderr, "timeout in stopkill(!)\n");
        unit_enter_state (unit, unit->target);
        return;
    }
    case S_PRESTART:
    {
        fprintf (stderr, "timeout in prestart\n");
        if (unit->rtype == R_YES)
        {
            fprintf (stderr, "restarting for prestart\n");
            unit_enter_prestart (unit);
        }
        else
        {
            unit->target = S_MAINTENANCE;
            unit_purge_and_target (unit);
        }
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
    { /* main PID has exited */
        unit->main_pid = 0;
        if (unit->timer_id)
            timer_del (unit->timer_id);
        /* if exit was S16_FATAL, go to maintenance instead - add this later */
        if (exit_was_abnormal (info->flags))
        {
            fprintf (stderr, "Bad exit in a main pid\n");
            if (unit->rtype == R_NO)
                unit->target = S_OFFLINE;
            /* if we were online, we'll go all the way back to the prestart
             * state, after purging any remaining PIDs. */
            else if (unit->state == S_ONLINE)
                unit->target = S_PRESTART;
            else
                unit->target = unit->state;

            unit_purge_and_target (unit);
        }
        else
        {
            switch (unit->state)
            {
            case S_PRESTART:
                /* the main PID exited from prestart; we'll clean up any
                 * remnants and then enter the start state. */
                unit->target = S_START;
                unit_purge_and_target (unit);
                break;
            case S_POSTSTART:
                /* we won't bother purging any PIDs left over by poststart. */
                unit_enter_online (unit);
                break;
            case S_ONLINE:
                if (unit->rtype == R_YES)
                    unit->target = S_PRESTART;
                else
                    unit->target = S_OFFLINE;
                unit_enter_stop (unit);
                break;
            case S_STOP:
                unit_purge_and_target (unit);
                break;
            }
            return;
        }
    }
    else if (info->event == PT_EXIT && info->pid == unit->secondary_pid)
    {
        if (unit->timer_id)
            timer_del (unit->timer_id);
        unit->secondary_pid = 0;
        unit_enter_online (unit);
    }

    switch (unit->state)
    {
    case S_STOP:
    case S_STOP_TERM:
    case S_STOP_KILL:
        if (!List_count (unit->pids))
        {
            timer_del (unit->timer_id);
            unit->main_pid = 0;
            unit->timer_id = 0;
            fprintf (stderr, "All PIDs purged\n");
            /* having purged all the PIDs we need to, we are free to enter the
             * targeted next state. */
            unit_enter_state (unit, unit->target);
        }

        break;
    }
}
