#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>

#include "s16db.h"

#include "manager.h"
#include "restartd_rpc.h"

manager_t Manager;

extern void s16_restartd_prog_1 (struct svc_req * rqstp,
                                 register SVCXPRT * transp);
void install_configd_svc ();

static int restartd_rpc_loop (void * userData) { svc_run (); }

/* Sends a message to the main event loop. */
void note_send (enum msg_type_e type, svc_id_t id, svc_id_t i_id, void * misc)
{
    struct kevent ev;
    msg_t * msg = malloc (sizeof (msg_t));

    msg->type = type;
    msg->id = id;
    msg->i_id = i_id;
    msg->misc = misc;

    /* In order to awaken the event loop, we may use EVFILT_USER, which
     * is a user-controlled kevent. We enable and trigger it here, after
     * we first secure the lock. After we have done our work, we can then
     * add to the message queue our message. */
    mtx_lock (&Manager.lock);
    memset (&ev, 0, sizeof (ev));
    EV_SET (&ev, NOTE_IDENT, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, 0, 0);
    mtx_unlock (&Manager.lock);

    if (kevent (Manager.kq, &ev, 1, NULL, 0, 0) == -1)
    {
        perror ("kevent! (trigger EVFILT_USER)");
    }
    else
        msg_list_add (Manager.msgs, msg);
}

/* Awakens the main event loop - no message. */
void note_awake ()
{
    struct kevent ev;
    mtx_lock (&Manager.lock);
    memset (&ev, 0, sizeof (ev));
    EV_SET (&ev, NOTE_AWAKE, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, 0, 0);
    mtx_unlock (&Manager.lock);
}

int main ()
{
    struct sockaddr_in addr;
    int sock;
    struct kevent sigfd, userev, ev;
    struct sigaction sa;
    struct timespec tmout = {0, /* return at once initially */
                             0};
    fd_set rpc_fds;

    subreap_acquire ();

    if ((Manager.kq = kqueue ()) == -1)
    {
        perror ("kqueue!");
        exit (EXIT_FAILURE);
    }

    EV_SET (&sigfd, SIGCHLD, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);

    if (kevent (Manager.kq, &sigfd, 1, 0, 0, 0) == -1)
    {
        perror ("kqueue! (sigfd installation)");
        exit (EXIT_FAILURE);
    }

    EV_SET (&userev, NOTE_IDENT, EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_FFNOP,
            0, 0);

    if (kevent (Manager.kq, &userev, 1, 0, 0, 0) == -1)
    {
        perror ("kqueue! (userev installation)");
        exit (EXIT_FAILURE);
    }

    EV_SET (&userev, NOTE_AWAKE, EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_FFNOP,
            0, 0);

    if (kevent (Manager.kq, &userev, 1, 0, 0, 0) == -1)
    {
        perror ("kqueue! (userev installation)");
        exit (EXIT_FAILURE);
    }

    Manager.ptrack = pt_new (Manager.kq);
    Manager.units = List_new ();
    Manager.clnt_cfg = s16db_context_create ();
    Manager.msgs = List_new ();
    Manager.timers = List_new ();
    mtx_init (&Manager.lock, mtx_plain);

    sa.sa_flags = 0;
    sigemptyset (&sa.sa_mask);
    sa.sa_handler = discard_signal;
    sigaction (SIGCHLD, &sa, NULL);

    sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        perror ("socket creation failed");
        exit (1);
    }
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof (int));

    addr.sin_family = AF_INET;
    addr.sin_port = htons (12280);
    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    if (bind (sock, (struct sockaddr *)&addr, sizeof addr) == -1)
    {
        perror ("bindinding socket failed");
        exit (1);
    }

    SVCXPRT * transp = svctcp_create (sock, 0, 0);
    if (!transp)
    {
        fprintf (stderr, "failed to create RPC service on TCP port 12288\n");
        exit (1);
    }

    if (!svc_register (transp, S16_RESTARTD_PROG, S16_RESTARTD_V1,
                       s16_restartd_prog_1, 0))
    {
        fprintf (stderr, "unable to register service\n");
        exit (1);
    }

    thrd_create (&Manager.thrd_rpc, restartd_rpc_loop, 0);

    if (!Manager.clnt_cfg)
    {
        install_configd_svc ();
        note_send (MSG_START, 1, 1, 0);
    }

    /* The main loop.
     * KEvent will return for signals, process events, and user events.
     */

    while (1)
    {
        pt_info_t * info;
        int i;

        memset (&ev, 0x00, sizeof (struct kevent));

        i = kevent (Manager.kq, NULL, 0, &ev, 1, &tmout);

        mtx_lock (&Manager.lock);
        EV_SET (&userev, NOTE_IDENT, EVFILT_USER, EV_DISABLE | EV_CLEAR,
                NOTE_FFCOPY, 0, 0);

        if (i == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror ("kevent! (i = -1 in loop)\n");
                sleep (1);
            }
        }

        tmout.tv_sec = 3;
        info = 0;

        if ((info = pt_investigate_kevent (Manager.ptrack, &ev)))
        {
            unit_t * unit = unit_find_by_pid (Manager.units, info->pid);

            if (!unit)
                unit = unit_find_by_pid (Manager.units, info->ppid);

            /*if (!unit)
                fprintf (stderr, "error: no unit associated with pid %d\n",
                         info->pid);*/
            else
                unit_ptevent (unit, info);

            free (info);
        }

        switch (ev.filter)
        {
        case EVFILT_USER:
        {
            if (ev.ident == NOTE_IDENT)
            {
                msg_t * msg;
                msg = msg_list_lpop (Manager.msgs);
                if (msg)
                {
                    unit_t * unit;
                    if (unit = unit_find (Manager.units, msg->id, msg->i_id))
                        unit_ctrl (unit, msg->type);
                }
                free (msg);
            }
            break;
        }
        case EVFILT_SIGNAL:
            printf ("Signal received: %d. Additional data: %d\n", ev.ident,
                    ev.data);
            if (ev.ident == SIGCHLD)
                while (waitpid ((pid_t) (-1), 0, WNOHANG) > 0)
                    ;
            break;
        case EVFILT_TIMER:
        {
            Timer * timer;
            printf ("Timer\n");
            if (timer = timer_find (ev.ident))
                timer->cb (timer->userData, ev.ident);
            printf ("%p\n", timer);
        }
        default:;
        }
        mtx_unlock (&Manager.lock);
    }

    return 0;
}
