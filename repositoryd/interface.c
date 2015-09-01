/* This file implements the RPC interface functions which manipulate the
 * repositoryd object. */

#include "repositoryd_rpc.h"
#include "internal.h"

int * insert_service_1_svc (char * str, struct svc_req * req)
{
    static int result;
    result = insert_svc (str);
    return &result;
}

int * delete_service_1_svc (svc_id_t id, struct svc_req * req)
{
    static int result;
    result = delete_svc (id);
    return &result;
}
