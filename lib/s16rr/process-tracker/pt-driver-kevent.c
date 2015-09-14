#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include "list.h"
#include "s16rr.h"

ListGenForNameType (pid, pid_t);

typedef struct process_tracker_s
{
    int kq;
    pid_list pids;
} process_tracker_t;

static pid_t * pid_new_p (pid_t val)
{
    pid_t * res = malloc (sizeof (pid_t));
    *res = val;
    return res;
}

process_tracker_t * pt_new (int kq)
{
    process_tracker_t * pt = malloc (sizeof (process_tracker_t));
    pt->kq = kq;
    pt->pids = List_new ();
    return pt;
}

int pt_watch_pid (process_tracker_t * pt, pid_t pid)
{
    int i;
    struct kevent ke;

    EV_SET (&ke, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT | NOTE_TRACK, 0, NULL);
    i = kevent (pt->kq, &ke, 1, NULL, 0, NULL);

    if (i == -1)
        fprintf (stderr, "Error: failed to watch PID %d: %s\n", pid,
                 strerror (errno));
    else
        pid_list_add (pt->pids, pid_new_p (pid));

    return i == -1 ? 1 : 0;
}

void pt_disregard_pid (process_tracker_t * pt, pid_t pid)
{
    struct kevent ke;
    pid_list_iterator it;

    EV_SET (&ke, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT | NOTE_TRACK, 0, NULL);
    kevent (pt->kq, &ke, 1, NULL, 0, NULL);

    for (it = pid_list_begin (pt->pids); it != NULL;
         pid_list_iterator_next (&it))
    {
        if (*it->val == pid)
        {
            free (it->val);
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

    if (ke->filter != EVFILT_PROC)
        return 0;

    if (ke->fflags & NOTE_CHILD)
    {
        printf ("new pid %d has %d as parent\n", ke->ident, ke->data);
        info.event = PT_CHILD;
        info.pid = ke->ident;
        info.ppid = ke->data;

        pid_list_add (pt->pids, pid_new_p (ke->ident));
    }
    else if (ke->fflags & NOTE_EXIT)
    {
        pid_list_iterator it;

        printf ("pid %d exited\n", ke->ident);
        info.event = PT_EXIT;
        info.pid = ke->ident;
        info.ppid = 0;
        info.flags = ke->data;

        for (it = pid_list_begin (pt->pids); it != NULL;
             pid_list_iterator_next (&it))
        {
            if (*it->val == ke->ident)
            {
                free (it->val);
                pid_list_del (pt->pids, it->val);
                break;
            }
        }
    }
    else
        return 0;

result:
    result = malloc (sizeof (pt_info_t));
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
    free (pt);
}
