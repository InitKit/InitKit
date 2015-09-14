#ifndef __S16_RR_H_
#define __S16_RR_H_

#include <sys/types.h>
#include <sys/event.h>
#include <sys/wait.h>

#ifdef __cplusplus
extern "C" {
#endif

/* process tracking structures */
typedef enum
{
    PT_CHILD,
    PT_EXIT,
} pt_event;

/* This structure contains details of a process tracker event.
 * In the case of child, the field 'ppid' contains the parent pid.
 * In the case of exit, the field 'flags' contains the exit data. */
typedef struct pt_info_s
{
    pt_event event;
    pid_t pid, ppid;
    long flags;
} pt_info_t;

typedef struct process_wait_s
{
    int fd[2];
    pid_t pid;
} process_wait_t;

typedef struct process_tracker_s process_tracker_t;

/* subreaping routines */

int subreap_acquire ();
int subreap_relinquish ();
int subreap_status ();

/* process basic routines */

/* Forks a process for the command specified.
 * This process waits until process_fork_continue()
 * is called. */
process_wait_t * process_fork_wait (const char * cmd_);
/* Tells the child process to continue.
 * This also frees the process_wait_t structure. */
void process_fork_continue (process_wait_t * pwait);

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

/* Returns 0 for a healthy exit, and either signal number or return code if not.
 */
int exit_was_abnormal (int wstat);

/* What is said on the tin, this does.
 * i.e. it does literally nothing functional. */
void discard_signal (int no);

#ifdef __cplusplus
}
#endif

#endif
