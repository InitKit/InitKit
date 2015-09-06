#ifndef __S16_RR_H_
#define __S16_RR_H_

#include <sys/wait.h>

#ifdef __cplusplus
extern "C" {
#endif

/* process tracking structures */

/* This structure contains details of a process tracker event.
 * In the case of child, the field 'flags' contains the parent pid.
 * In the case of exit, the field 'flags' contains the exit data. */
typedef struct pt_info_s
{
    enum
    {
        CHILD,
        EXIT,
    } event;
    pid_t pid;
    long flags;
} pt_info_t;

typedef struct process_tracker_s process_tracker_t;

/* subreaping routines */

int subreap_acquire ();
int subreap_relinquish ();
int subreap_status ();

/* process basic routines */

/* Retrieves the parent PID of a PID. */
pid_t process_get_ppid (pid_t pid);

/* process tracking routines */

/* Creates a new process tracker and returns a handle to it.
 * Passed the KQueue descriptor that it may register its events. */
process_tracker_t * pt_new (int kq);

/* Adds a PID to the watchlist. */
int pt_watch_pid (process_tracker_t * pt, pid_t pid);

/* Removes a PID from the watchlist. */
void pt_disregard_pid (process_tracker_t * pt, pid_t pid);

/* Investigates a kevent, that it may determine if it contains
 * an event that is relevant to the process tracker.
 *
 * Returns null for irrelevant, and a pointer to a pt_info_t for
 * a relevant process tracker event.
 *
 * In the event of a child or an exit, there is no need to manually
 * add or remove, respectively, that PID from the watchlist - this is
 * automatically done by the tracker. */
pt_info_t * pt_investigate_kevent (process_tracker_t * pt, struct kevent * ke);

/* Destroys a process tracker.
 * Requires KQueue descriptor to remove itself from the queue. */
void pt_destroy (process_tracker_t * pt);

/* misc routines */

/* What is said on the tin, this does.
 * i.e. it does literally nothing functional. */
void discard_signal (int no);

#ifdef __cplusplus
}
#endif

#endif
