#include "s16db.h"

rpc_property_t property_to_rpc_property (property_t * prop)
{
    rpc_property_t newRpc_property;

    newRpc_property.id = prop->id;
    newRpc_property.name = strdup (prop->name);
    newRpc_property.value.type = prop->value.type;
    if (prop->value.type == STRING)
        newRpc_property.value.property_value_u_u.s =
            strdup (prop->value.property_value_u_u.s);
    else
        newRpc_property.value.property_value_u_u.i =
            prop->value.property_value_u_u.i;

    return newRpc_property;
}

rpc_property_t * property_list_to_rpc_property_array (property_t * box)
{
    register unsigned int p_index = 0;
    property_t * p_iter, *p_tmp;
    rpc_property_t * newRpc_plist =
        malloc (HASH_COUNT (box) * sizeof (rpc_property_t));

    HASH_ITER (hh, box, p_iter, p_tmp)
    {
        newRpc_plist[p_index++] = property_to_rpc_property (p_iter);
    }
}

rpc_svc_instance_t svc_instance_to_rpc_svc_instance (svc_instance_t * inst)
{
    rpc_svc_instance_t newRpc_instance;

    newRpc_instance.id = inst->id;
    newRpc_instance.name = strdup (inst->name);
    newRpc_instance.svc_id = inst->svc_id;
    newRpc_instance.properties.properties_len = HASH_COUNT (inst->properties);
    newRpc_instance.properties.properties_val =
        property_list_to_rpc_property_array (inst->properties);
}

rpc_svc_t * svc_to_rpc_svc (svc_t * svc)
{
    register unsigned int i_index = 0, i_cnt = 0, p_index = 0, p_cnt = 0;
    rpc_svc_t * newRpc_svc = malloc (sizeof (rpc_svc_t));
    svc_instance_t * i_iter, *i_tmp;

    newRpc_svc->id = svc->id;
    newRpc_svc->name = strdup (svc->name);

    i_cnt = HASH_COUNT (svc->instances);
    newRpc_svc->instances.instances_len = i_cnt;
    newRpc_svc->instances.instances_val =
        malloc (i_cnt * sizeof (rpc_svc_instance_t));

    HASH_ITER (hh, svc->instances, i_iter, i_tmp)
    newRpc_svc->instances.instances_val[i_index++] =
        svc_instance_to_rpc_svc_instance (i_iter);

    newRpc_svc->properties.properties_len = HASH_COUNT (svc->properties);
    newRpc_svc->properties.properties_val =
        property_list_to_rpc_property_array (svc->properties);

    return newRpc_svc;
}
