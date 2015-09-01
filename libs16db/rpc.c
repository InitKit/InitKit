#include "repositoryd_rpc.h"
#include "s16db.h"

#define RETURN_OR_FAIL(call)                                                   \
    int * result = call;                                                       \
    if (result == NULL)                                                        \
    {                                                                          \
        clnt_perror (clnt, "local");                                           \
        exit (3);                                                              \
    }                                                                          \
    else                                                                       \
        return *result;

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

int s16db_insert_service (CLIENT * clnt, char const * name)
{
    RETURN_OR_FAIL (insert_service_1 ((char *)name, clnt));
}

int s16db_delete_service (CLIENT * clnt, svc_id_t id)
{
    RETURN_OR_FAIL (delete_service_1 (id, clnt));
}

svc_t * s16db_retrieve_service (CLIENT * clnt, svc_id_t id)
{
    return rpc_svc_to_svc (retrieve_service_1 (id, clnt));
}
