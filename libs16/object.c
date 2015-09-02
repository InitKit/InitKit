#include "s16.h"
#include "s16db.h"

#define gen_find_id_wrapper(type)                                              \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr)         \
    {                                                                          \
        type##_t * i;                                                          \
        HASH_FIND_INT (box, keyptr, i);                                        \
        return i;                                                              \
    }

#define gen_find_name_wrapper(type)                                            \
    type##_t * type##_find_name (type##_t * box, char const * name)            \
    {                                                                          \
        type##_t * i = box;                                                    \
        for (i; i != NULL; i = i->hh.next)                                     \
            if (!strcmp (i->name, name))                                       \
                return i;                                                      \
        return 0;                                                              \
    }

gen_find_id_wrapper (svc);
gen_find_name_wrapper (svc);

gen_find_id_wrapper (property);
gen_find_name_wrapper (property);

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

    HASH_ADD_INT (Svc->properties, id, newProp);
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

    HASH_ADD_INT (Svc->properties, id, newProp);
}

void destroy_property (property_t * delProperty)
{
    if (delProperty->value.type == STRING)
        free (delProperty->value.pval_u.s);
    free (delProperty);
}

void destroy_properties_list (property_t * box)
{
    property_t * i_tmp, *i_iter;
    HASH_ITER (hh, box, i_iter, i_tmp)
    {
        destroy_property (i_iter);
        HASH_DEL (box, i_iter);
    }
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
