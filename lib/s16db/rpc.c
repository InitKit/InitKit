#include <stdlib.h>
#include "repositoryd_rpc.h"
#include "s16db.h"
#include "translate.h"

#define RETURN_OR_FAIL(call)                                                   \
    long * result = (long *)call;                                              \
    if (result == NULL)                                                        \
    {                                                                          \
        clnt_perror (clnt, "local");                                           \
        return 0;                                                              \
    }                                                                          \
    else                                                                       \
        return result;

#define RETURN_IF_CLNT_ZERO()                                                  \
    if (clnt == 0)                                                             \
        return 0;

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
    return clnt;
}

void s16db_context_destroy (CLIENT * clnt) { clnt_destroy (clnt); }

svc_id_t * s16db_svc_insert (CLIENT * clnt, char const * name)
{
    // RETURN_IF_CLNT_ZERO();
    RETURN_OR_FAIL (svc_insert_1 ((char *)name, clnt));
}

svc_id_t * s16db_svc_install (CLIENT * clnt, svc_t * svc)
{
    RETURN_OR_FAIL (svc_install_1 (svc_to_rpc_svc (svc), clnt));
}

int * s16db_svc_delete (CLIENT * clnt, svc_id_t id)
{
    RETURN_OR_FAIL (svc_delete_1 (id, clnt));
}

svc_t * s16db_svc_retrieve (CLIENT * clnt, svc_id_t id)
{
    return rpc_svc_to_svc (svc_retrieve_1 (id, clnt));
}

svc_t * s16db_svc_retrieve_by_name (CLIENT * clnt, char * id)
{
    return rpc_svc_to_svc (svc_retrieve_by_name_1 (id, clnt));
}

svc_list s16db_svc_retrieve_all (CLIENT * clnt)
{
    rpc_svc_array_t * arr = svc_retrieve_all_1 (clnt);
    svc_list box;

    if (!arr)
        return 0;

    box = rpc_svc_array_to_svc_list (arr);
    return box;
}

int * s16db_svc_set_property_int (CLIENT * clnt, svc_id_t id, char const * key,
                                  long value)
{
    RETURN_OR_FAIL (svc_set_property_int_1 (id, (char *)key, value, clnt));
}

int * s16db_svc_set_property_string (CLIENT * clnt, svc_id_t id,
                                     char const * key, char const * value)
{
    RETURN_OR_FAIL (
        svc_set_property_string_1 (id, (char *)key, (char *)value, clnt));
}
