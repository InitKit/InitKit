#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>

#include "s16db.h"

#include "svcorder.h"
#include "config-subscriber_rpc.h"

#define PORT 12284

svcorder_t Svcorder;

extern void s16_config_subscriber_prog_1 (struct svc_req * rqstp,
                                          register SVCXPRT * transp);

static void fd_set_to_kq (int kq, fd_set * set, int del)
{
    struct kevent fd_ev;
    for (int fd = 0; fd < FD_SETSIZE; fd++)
    {
        if (FD_ISSET (fd, set))
        {
            printf ("FD %d is set\n", fd);
            if (!del)
                EV_SET (&fd_ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
            else
                EV_SET (&fd_ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent (kq, &fd_ev, 1, NULL, 0, NULL);
        }
    }
}

int main ()
{
    struct sockaddr_in addr;
    int sock;
    struct kevent ev;
    fd_set rpc_fds;
    SVCXPRT * transp;

    if (!(Svcorder.clnt_cfg = s16db_context_create ()))
        error_fatal ("Failed to connect to repositoryd!\n");

    if ((Svcorder.kq = kqueue ()) == -1)
        perror_fatal ("kqueue! (sigfd installation)");

    if (!(sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)))
        perror_fatal ("socket creation failed");

    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof (int)) ==
        -1)
        perror_fatal ("setsockopt failed\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT);
    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    if (bind (sock, (struct sockaddr *)&addr, sizeof addr) == -1)
        perror_fatal ("binding socket failed");

    if (!(transp = svctcp_create (sock, 0, 0)))
        error_fatal ("failed to create RPC service on TCP port 12284\n");

    if (!svc_register (transp, S16_CONFIG_SUBSCRIBER_PROG,
                       S16_CONFIG_SUBSCRIBER_V1, s16_config_subscriber_prog_1,
                       0))
        error_fatal ("unable to register service S16_RESTARTD_PROG\n");

    s16db_config_register_port (Svcorder.clnt_cfg, PORT);
    s16db_config_subscribe_status (Svcorder.clnt_cfg, PORT, 0, 0);

    rpc_fds = svc_fdset;
    fd_set_to_kq (Svcorder.kq, &rpc_fds, 0);

    while (1)
    {
        int i;

        memset (&ev, 0x00, sizeof (struct kevent));

        i = kevent (Svcorder.kq, NULL, 0, &ev, 1, 0);

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

        switch (ev.filter)
        {
        case EVFILT_READ:
            if (FD_ISSET (ev.ident, &rpc_fds))
            { /* a procedure we export has been called. */
                fd_set_to_kq (Svcorder.kq, &rpc_fds, 1);
                svc_getreqset (&rpc_fds);
                fd_set_to_kq (Svcorder.kq, &rpc_fds, 0);
            }
            break;
        }
    }
    return 0;

    return 0;
}
