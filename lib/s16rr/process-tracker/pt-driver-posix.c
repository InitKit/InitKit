
/* This is a POSIX-compliant process-tracker driver.
 * It can only detect immediate childrens' exits, as it uses
 * SIGCHLD only. */

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>

#include "list.h"
#include "s16rr.h"

ListGenForNameType (pid, pid_t);

void * s16mem_alloc (unsigned long);
void s16mem_free (void *);

typedef struct process_tracker_s
{
    int kq;
    pid_list pids;
} process_tracker_t;

static pid_t * pid_new_p (pid_t val)
{
    pid_t * res = s16mem_alloc (sizeof (pid_t));
    *res = val;
    return res;
}

process_tracker_t * pt_new (int kq)
{
    process_tracker_t * pt = s16mem_alloc (sizeof (process_tracker_t));
    pt->kq = kq;
    pt->pids = List_new ();
    return pt;
}

int pt_watch_pid (process_tracker_t * pt, pid_t pid)
{
    pid_list_add (pt->pids, pid_new_p (pid));
    return 0;
}

void pt_disregard_pid (process_tracker_t * pt, pid_t pid)
{
    pid_list_iterator it;

    for (it = pid_list_begin (pt->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        if (*it->val == pid)
        {
            s16mem_free (it->val);
            pid_list_del (pt->pids, it->val);
            break;
        }
    }

    return;
}

pt_info_t * pt_investigate_kevent (process_tracker_t * pt, struct kevent * ke)
{
    pt_info_t * result;
    pt_info_t info;

    if (ke->filter != EVFILT_SIGNAL)
        return 0;

    if (ke->ident == SIGCHLD)
    {
        info.event = PT_EXIT;
        info.pid = waitpid ((pid_t) (-1), (int *)&info.flags, WNOHANG);
        info.ppid = 0;
    }
    else
        return 0;

    result = s16mem_alloc (sizeof (pt_info_t));
    *result = info;

    return result;
}

void pt_destroy (process_tracker_t * pt)
{
    for (pid_list_iterator it = pid_list_begin (pt->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        pt_disregard_pid (pt, *it->val);
        free (it->val);
    }

    List_destroy (pt->pids);
    s16mem_free (pt);
}
