#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "s16.h"

repositoryd_t RD;

extern void s16_repositoryd_prog_1 (struct svc_req * rqstp,
                                    register SVCXPRT * transp);

int main (int argc, char * argv[])
{
    RD.services = List_new ();
    RD.subscribers = List_new ();

    int sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        perror ("socket creation failed");
        exit (1);
    }
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof (int));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons (12288);
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

    if (!svc_register (transp, S16_REPOSITORYD_PROG, S16_REPOSITORYD_V1,
                       s16_repositoryd_prog_1, 0))
    {
        fprintf (stderr, "unable to register service\n");
        exit (1);
    }

    svc_run ();
    printf ("svc_run should not exit\n");
    return 0;
}
