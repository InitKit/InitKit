/* This file implements the RPC interface functions which manipulate the
 * repositoryd object. */

#include "s16db.h"
#include "translate.h"
#include "internal.h"

int * svc_insert_1_svc (char * str, struct svc_req * req)
{
    static int result;
    result = insert_svc (str);
    return &result;
}

int * svc_delete_1_svc (svc_id_t id, struct svc_req * req)
{
    static int result;
    result = delete_svc (id);
    return &result;
}

rpc_svc_t * svc_retrieve_1_svc (svc_id_t id, struct svc_req * req)
{
    return svc_to_rpc_svc (svc_find_id (RD.services, &id));
}

int * svc_set_property_int_1_svc (svc_id_t id, char * key, long value,
                                  struct svc_req * req)
{
    static int result;
    result = set_svc_property_int (id, key, value);
    return &result;
}

int * svc_set_property_string_1_svc (svc_id_t id, char * key, char * value,
                                     struct svc_req * req)
{
    static int result;
    result = set_svc_property_string (id, key, value);
    return &result;
}
