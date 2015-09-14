/* This file implements the RPC interface functions which manipulate the
 * repositoryd object. */

#include "s16db.h"
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
    svc_t *iter, *tmp;
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

svc_id_t * instance_create_1_svc (svc_id_t id, char * name,
                                  struct svc_req * req)
{
    static svc_id_t result;
    svc_t * svc;

    if ((!svc_find_id (RD.services, id)))
        return 0;

    if (strcmp (name, "default"))
        result = s16_inst_new_default (svc);
    else
        result = s16_inst_new (svc, name);

    return &result;
}

static svc_instance_t * _find_instance (svc_id_t id, svc_id_t i_id)
{
    svc_t * svc = svc_find_id (RD.services, id);
    svc_instance_t * inst;

    if (!svc)
        return 0;

    inst = inst_find_id (svc->instances, i_id);
    return inst;
}

rpc_svc_instance_t * instance_retrieve_1_svc (svc_id_t id, svc_id_t i_id,
                                              struct svc_req * req)
{
    static rpc_svc_instance_t result;
    svc_instance_t * inst;

    if (!(inst = _find_instance (id, i_id)))
        return 0;

    result = svc_instance_to_rpc_svc_instance (inst);

    return &result;
}

int * instance_set_property_int_1_svc (svc_id_t id, svc_id_t i_id, char * key,
                                       long value, struct svc_req * req)
{
    static int result;
    svc_instance_t * inst;

    if (!(inst = _find_instance (id, i_id)))
    {
        result = 1;
        return &result;
    }

    inst_object_set_property_int (inst, key, value);

    result = 0;
    return &result;
}

int * instance_set_property_string_1_svc (svc_id_t id, svc_id_t i_id,
                                          char * key, char * value,
                                          struct svc_req * req)
{
    static int result;
    svc_instance_t * inst;

    if (!(inst = _find_instance (id, i_id)))
    {
        result = 1;
        return &result;
    }

    inst_object_set_property_string (inst, key, value);

    result = 0;
    return &result;
}

int * config_register_1_svc (struct svc_req * req)
{
    static int result;
    result = i_config_register ();
    return &result;
}

int * config_subscribe_status_1_svc (int p, svc_id_t id, svc_id_t i_id,
                                     struct svc_req * req)
{
    static int result;
    result = i_config_subscribe_status (p, id, i_id);
    return &result;
}
