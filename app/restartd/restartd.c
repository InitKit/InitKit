#include <stdio.h>
#include "restartd_rpc.h"
#include "manager.h"

manager_t Manager;

extern void s16_restartd_prog_1 (struct svc_req * rqstp,
                                    register SVCXPRT * transp);

int restartd_rpc_loop(void * userData)
{

}

int main ()
{
    int sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        perror ("socket creation failed");
        exit (1);
    }
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof (int));

    struct sockaddr_in addr;
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

    thrd_create(&Manager.rpc_thread, restartd_rpc_loop, 0);

    return 0;
}
