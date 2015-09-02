#include <stdlib.h>
#include <string.h>
#include "s16.h"
#include "s16db.h"

#define gen_find_id_wrapper(name, type)                                        \
    type * name##_find_id (name##_list box, unsigned long key)                 \
    {                                                                          \
        name##_list_iterator * i;                                              \
        for (i = name##_list_begin (box); i != NULL;                           \
             name##_list_iterator_next (&i))                                   \
            if (i->val->id == key)                                             \
                return i->val;                                                 \
        return 0;                                                              \
    }

#define gen_find_name_wrapper(Name, type)                                      \
    type * Name##_find_name (Name##_list box, char const * nam)                \
    {                                                                          \
        Name##_list_iterator * i;                                              \
        for (i = Name##_list_begin (box); i != NULL;                           \
             Name##_list_iterator_next (&i))                                   \
            if (!strcmp (i->val->name, nam))                                   \
                return i->val;                                                 \
        return 0;                                                              \
    }

gen_find_id_wrapper (svc, svc_t);
gen_find_name_wrapper (svc, svc_t);

gen_find_id_wrapper (prop, property_t);
gen_find_name_wrapper (prop, property_t);

void svc_object_set_property_string (svc_t * Svc, const char * key,
                                     const char * value)
{
    if ((!key) | (!Svc))
        return;
    DestroyPropIfExists (Svc->properties, key);
    svc_id_t rnum;
    property_t * newProp = calloc (1, sizeof (property_t));

    newProp->name = strdup (key);
    while (property_find_id (Svc->properties, &rnum))
        rnum = rand ();
    newProp->id = rnum;
    newProp->value.type = STRING;
    newProp->value.pval_u.s = strdup (value);

    prop_list_add (Svc->properties, newProp);
}

void svc_object_set_property_int (svc_t * Svc, const char * key, long value)
{
    if ((!key) | (!Svc))
        return;
    DestroyPropIfExists (Svc->properties, key);
    svc_id_t rnum;
    property_t * newProp = calloc (1, sizeof (property_t));

    newProp->name = strdup (key);
    while (property_find_id (Svc->properties, &rnum))
        rnum = rand ();
    newProp->id = rnum;
    newProp->value.type = NUMBER;
    newProp->value.pval_u.i = value;

    prop_list_add (Svc->properties, newProp);
}

void destroy_property (property_t * delProperty)
{
    if (delProperty->value.type == STRING)
        free (delProperty->value.pval_u.s);
    free (delProperty);
}

void destroy_properties_list (property_t * box)
{
    /*property_t * i_tmp, *i_iter;
    HASH_ITER (hh, box, i_iter, i_tmp)
    {
        HASH_DEL (box, i_iter);
        destroy_property (i_iter);
    }*/
}

void destroy_instance (svc_instance_t * delInstance)
{
    destroy_properties_list (delInstance->properties);
    free (delInstance->name);
    free (delInstance);
}

void destroy_svc (svc_t * delSvc)
{
    svc_instance_t * i_tmp, *i_iter;
    HASH_ITER (hh, delSvc->instances, i_iter, i_tmp)
    {
        destroy_instance (i_iter);
    }
    destroy_properties_list (delSvc->properties);
    HASH_CLEAR (hh, delSvc->instances);
    free (delSvc->name);
    free (delSvc);
}

void destroy_svcs_list (svc_t * box)
{
    svc_t * s_tmp, *s_iter;
    HASH_ITER (hh, box, s_iter, s_tmp)
    {
        HASH_DEL (box, s_iter);
        destroy_svc (s_iter);
    }
}
