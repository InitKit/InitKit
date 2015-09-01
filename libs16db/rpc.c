#include "s16db.h"

CLIENT * s16db_context_create ()
{
    struct sockaddr_in addr;
    int sock = RPC_ANYSOCK;
    CLIENT * clnt;

    addr.sin_family = AF_INET;
    addr.sin_port = htons (12288);
    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    clnt = clnttcp_create (&addr, S16_REPOSITORYD_PROG, S16_REPOSITORYD_V1,
                           &sock, 0, 0);
    if (clnt == NULL)
    {
        clnt_pcreateerror ("localhost");
        return 0;
    }
}

void s16db_context_destroy (CLIENT * clnt) { clnt_destroy (clnt); }
