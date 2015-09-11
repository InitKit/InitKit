#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>

#include "manager.h"
#include "restartd_rpc.h"

manager_t Manager;

extern void s16_restartd_prog_1 (struct svc_req * rqstp,
                                 register SVCXPRT * transp);

static int restartd_rpc_loop (void * userData) { svc_run (); }

int main ()
{
    struct sockaddr_in addr;
    int sock;
    struct kevent sigfd, ev;
    struct sigaction sa;
    struct timespec tmout = {0, /* return at once initially */
                             0};
    fd_set rpc_fds;

    subreap_acquire ();

    if ((Manager.kq = kqueue ()) == -1)
    {
        perror ("kqueue");
        exit (EXIT_FAILURE);
    }

    EV_SET (&sigfd, SIGCHLD, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);

    if (kevent (Manager.kq, &sigfd, 1, 0, 0, 0) == -1)
    {
        perror ("kqueue");
        exit (EXIT_FAILURE);
    }

    Manager.ptrack = pt_new (Manager.kq);

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

    /* The main loop.
     * KEvent will return for signals and process events.
     */

    while (1)
    {
        pt_info_t * info;
        int i;

        memset (&ev, 0x00, sizeof (struct kevent));

        i = kevent (Manager.kq, NULL, 0, &ev, 1, &tmout);
        if (i == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror ("kevent! (i = -1 in loop)\n");
            }
        }

        tmout.tv_sec = 3;

        if (!(info = pt_investigate_kevent (Manager.ptrack, &ev)))
            goto post_pinfo;

    post_pinfo:
        switch (ev.filter)
        {
        case EVFILT_SIGNAL:
            printf ("Signal received: %d. Additional data: %d\n", ev.ident,
                    ev.data);
            break;
        }
    }

    return 0;
}
