#include "s16db.h"

#define RETURN_IF_NULL(val)                                                    \
    if (val == NULL)                                                           \
        return NULL;

rpc_property_t property_to_rpc_property (property_t * prop)
{
    rpc_property_t newRpc_property;

    newRpc_property.id = prop->id;
    newRpc_property.name = strdup (prop->name);
    newRpc_property.value.type = prop->value.type;
    if (prop->value.type == STRING)
        newRpc_property.value.pval_u.s = strdup (prop->value.pval_u.s);
    else
        newRpc_property.value.pval_u.i = prop->value.pval_u.i;

    return newRpc_property;
}

rpc_property_t * property_list_to_rpc_property_array (property_t * box)
{
    register unsigned p_index = 0;
    property_t * p_iter, *p_tmp;
    rpc_property_t * newRpc_plist =
        malloc (HASH_COUNT (box) * sizeof (rpc_property_t));

    HASH_ITER (hh, box, p_iter, p_tmp)
    {
        printf("%s %d\n", p_iter->name, HASH_COUNT(box));
        newRpc_plist[p_index++] = property_to_rpc_property (p_iter);
    }

    return newRpc_plist;
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

    return newRpc_instance;
}

rpc_svc_t svc_to_rpc_svc (svc_t * svc)
{
    register unsigned i_index = 0, i_cnt = 0, p_index = 0, p_cnt = 0;
    rpc_svc_t newRpc_svc;
    svc_instance_t * i_iter, *i_tmp;

    newRpc_svc.id = svc->id;
    newRpc_svc.name = strdup (svc->name);

    i_cnt = HASH_COUNT (svc->instances);
    newRpc_svc.instances.instances_len = i_cnt;
    newRpc_svc.instances.instances_val =
        malloc (i_cnt * sizeof (rpc_svc_instance_t));

    HASH_ITER (hh, svc->instances, i_iter, i_tmp)
    {
        newRpc_svc.instances.instances_val[i_index++] =
            svc_instance_to_rpc_svc_instance (i_iter);
    }

    newRpc_svc.properties.properties_len = HASH_COUNT (svc->properties);
    newRpc_svc.properties.properties_val =
        property_list_to_rpc_property_array (svc->properties);

    return newRpc_svc;
}

rpc_svc_t * svc_list_to_rpc_svc_array (svc_t * box)
{
    register unsigned s_index = 0;
    svc_t * s_iter, *s_tmp;
    rpc_svc_t * newRpc_svclist = malloc (HASH_COUNT (box) * sizeof (rpc_svc_t));

    HASH_ITER (hh, box, s_iter, s_tmp)
    {
        newRpc_svclist[s_index++] = svc_to_rpc_svc (s_iter);
    }

    return newRpc_svclist;
}

property_t * rpc_property_to_property (rpc_property_t * rprop)
{
    RETURN_IF_NULL (rprop);
    property_t * newProp = calloc (1, sizeof (property_t));

    newProp->id = rprop->id;
    newProp->name = strdup (rprop->name);
    if (rprop->value.type == STRING)
    {
        newProp->value.pval_u.s = strdup (rprop->value.pval_u.s);
    }
    else
        newProp->value.pval_u.i = rprop->value.pval_u.i;

    return newProp;
}

property_t * rpc_property_array_to_property_list (rpc_property_t rplist[],
                                                  unsigned int length)
{
    RETURN_IF_NULL (rplist);
    register unsigned rp_index;
    property_t * box = 0;

    if (length == 0)
        return box;

    for (rp_index = 0; rp_index < length; rp_index++)
    {
        printf("Add: %s\n", rplist[rp_index].name);
        HASH_ADD_INT (box, id, rpc_property_to_property (&rplist[rp_index]));
    }

    return box;
}

svc_instance_t * rpc_svc_instance_to_svc_instance (rpc_svc_instance_t * rinst)
{
    RETURN_IF_NULL (rinst);
    svc_instance_t * newInst = calloc (1, sizeof (svc_instance_t));

    newInst->id = rinst->id;
    newInst->name = strdup (rinst->name);
    newInst->svc_id = rinst->svc_id;
    newInst->properties = rpc_property_array_to_property_list (
        rinst->properties.properties_val, rinst->properties.properties_len);

    return newInst;
}

svc_t * rpc_svc_to_svc (rpc_svc_t * rsvc)
{
    RETURN_IF_NULL (rsvc);
    register unsigned i_index;
    svc_t * newSvc = calloc (1, sizeof (svc_instance_t));

    newSvc->id = rsvc->id;
    newSvc->name = strdup (rsvc->name);
    newSvc->properties = rpc_property_array_to_property_list (
        rsvc->properties.properties_val, rsvc->properties.properties_len);
    newSvc->instances = 0;

    if (rsvc->instances.instances_len > 0)
    {
        for (i_index = 0; i_index < rsvc->instances.instances_len; i_index++)
            HASH_ADD_INT (newSvc->instances, id,
                          rpc_svc_instance_to_svc_instance (
                              &rsvc->instances.instances_val[i_index]));
    }

    return newSvc;
}

svc_t * rpc_svc_array_to_svc_list (rpc_svc_array_t * svcArray)
{
    RETURN_IF_NULL (svcArray);
    register unsigned rs_index, length = svcArray->rpc_svc_array_t_len;
    svc_t * box = 0;

    if (length == 0)
        return box;

    for (rs_index = 0; rs_index < length; rs_index++)
    {
        svc_t * newSvc =
            rpc_svc_to_svc (&svcArray->rpc_svc_array_t_val[rs_index]);
        HASH_ADD_INT (box, id, newSvc);
    }

    return box;
}
