#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s16.h"
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

rpc_property_t * property_list_to_rpc_property_array (prop_list box)
{
    register unsigned p_index = 0;
    rpc_property_t * newRpc_plist;

    if (List_count (box) == 0)
        return 0;

    newRpc_plist = malloc (List_count (box) * sizeof (rpc_property_t));

    for (prop_list_iterator it = prop_list_begin (box); it != NULL;
         prop_list_iterator_next (&it))
    {
        newRpc_plist[p_index++] = property_to_rpc_property (it->val);
    }

    return newRpc_plist;
}

rpc_svc_instance_t svc_instance_to_rpc_svc_instance (svc_instance_t * inst)
{
    rpc_svc_instance_t newRpc_instance;

    newRpc_instance.id = inst->id;
    newRpc_instance.name = strdup (inst->name);
    newRpc_instance.svc_id = inst->svc_id;
    newRpc_instance.properties.properties_len = List_count (inst->properties);
    newRpc_instance.properties.properties_val =
        property_list_to_rpc_property_array (inst->properties);

    return newRpc_instance;
}

rpc_svc_t svc_to_rpc_svc (svc_t * svc)
{
    register unsigned i_cnt = 0, i_index = 0, p_cnt = 0;
    rpc_svc_t newRpc_svc;

    if (!svc)
    {
        newRpc_svc.name = strdup ("NULL");
        return newRpc_svc;
    }

    newRpc_svc.id = svc->id;
    newRpc_svc.name = strdup (svc->name);

    i_cnt = List_count (svc->instances);
    newRpc_svc.instances.instances_len = i_cnt;

    if (i_cnt > 0)
    {
        newRpc_svc.instances.instances_val =
            malloc (i_cnt * sizeof (rpc_svc_instance_t));
        for (inst_list_iterator it = inst_list_begin (svc->instances);
             it != NULL; inst_list_iterator_next (&it))
        {
            newRpc_svc.instances.instances_val[i_index++] =
                svc_instance_to_rpc_svc_instance (it->val);
        }
    }
    else
        newRpc_svc.instances.instances_val = 0;

    newRpc_svc.properties.properties_len = List_count (svc->properties);
    newRpc_svc.properties.properties_val =
        property_list_to_rpc_property_array (svc->properties);

    return newRpc_svc;
}

rpc_svc_t * svc_list_to_rpc_svc_array (svc_list box)
{
    register unsigned s_index = 0;
    rpc_svc_t * newRpc_svclist;

    if (List_count (box) == 0)
        return 0;

    newRpc_svclist = malloc (List_count (box) * sizeof (rpc_svc_t));

    for (svc_list_iterator it = svc_list_begin (box); it != NULL;
         svc_list_iterator_next (&it))
    {
        newRpc_svclist[s_index++] = svc_to_rpc_svc (it->val);
    }

    return newRpc_svclist;
}

property_t * rpc_property_to_property (rpc_property_t * rprop)
{
    RETURN_IF_NULL (rprop);
    property_t * newProp = calloc (1, sizeof (property_t));

    newProp->id = rprop->id;
    newProp->name = strdup (rprop->name);
    newProp->value.type = rprop->value.type;
    if (rprop->value.type == STRING)
        newProp->value.pval_u.s = strdup (rprop->value.pval_u.s);
    else
        newProp->value.pval_u.i = rprop->value.pval_u.i;

    return newProp;
}

prop_list rpc_property_array_to_property_list (rpc_property_t rplist[],
                                               unsigned int length)
{
    RETURN_IF_NULL (rplist);
    register unsigned rp_index;
    prop_list box = List_new ();

    if (length == 0)
        return box;

    for (rp_index = 0; rp_index < length; rp_index++)
    {
        prop_list_add (box, rpc_property_to_property (&rplist[rp_index]));
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
    svc_t * newSvc = s16_svc_new ();

    newSvc->id = rsvc->id;
    newSvc->name = strdup (rsvc->name);
    newSvc->properties = rpc_property_array_to_property_list (
        rsvc->properties.properties_val, rsvc->properties.properties_len);
    newSvc->instances = 0;

    if (rsvc->instances.instances_len > 0)
    {
        for (i_index = 0; i_index < rsvc->instances.instances_len; i_index++)
            inst_list_add (newSvc->instances,
                           rpc_svc_instance_to_svc_instance (
                               &rsvc->instances.instances_val[i_index]));
    }

    return newSvc;
}

svc_list rpc_svc_array_to_svc_list (rpc_svc_array_t * svcArray)
{
    RETURN_IF_NULL (svcArray);
    register unsigned rs_index, length = svcArray->rpc_svc_array_t_len;
    svc_list box = List_new ();

    if (length == 0)
        return box;

    for (rs_index = 0; rs_index < length; rs_index++)
    {
        svc_t * newSvc =
            rpc_svc_to_svc (&svcArray->rpc_svc_array_t_val[rs_index]);
        svc_list_add (box, newSvc);
    }

    return box;
}
