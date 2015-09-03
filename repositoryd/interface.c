/* This file implements the RPC interface functions which manipulate the
 * repositoryd object. */

#include "s16db.h"
#include "translate.h"
#include "internal.h"

svc_id_t * svc_insert_1_svc (char * str, struct svc_req * req)
{
    static svc_id_t result;
    result = insert_svc (str);
    return &result;
}

svc_id_t * svc_install_1_svc (rpc_svc_t rsvc, struct svc_req * req)
{
    static svc_id_t result;
    result = install_svc (rpc_svc_to_svc (&rsvc));
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
    static rpc_svc_t result;
    result = svc_to_rpc_svc (svc_find_id (RD.services, id));
    return &result;
}

rpc_svc_t * svc_retrieve_by_name_1_svc (char * id, struct svc_req * req)
{
    static rpc_svc_t result;
    result = svc_to_rpc_svc (svc_find_name (RD.services, id));
    return &result;
}

rpc_svc_array_t * svc_retrieve_all_1_svc (struct svc_req * req)
{
    static rpc_svc_array_t result;
    svc_t * iter, *tmp;
    result.rpc_svc_array_t_len = count_svcs ();
    result.rpc_svc_array_t_val =
        svc_list_to_rpc_svc_array (retrieve_all_svcs ());
    return &result;
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
