#include "storage.h"
#include "internal.h"

#define gen_find_id_wrapper(type)                                              \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr)         \
    {                                                                          \
        type##_t * i;                                                          \
        HASH_FIND_INT (box, keyptr, i);                                        \
        return i;                                                              \
    }

#define gen_find_name_wrapper(type)                                            \
    type##_t * type##_find_name (type##_t * box, char * name)                  \
    {                                                                          \
        type##_t * i = box;                                                    \
        for (i; i != NULL; i = i->hh.next)                                     \
            if (!strcmp (i->name, name))                                       \
                return i;                                                      \
        return 0;                                                              \
    }

gen_find_id_wrapper (rd_svc);
gen_find_name_wrapper (rd_svc);

int insert_svc (char const * name)
{
    rd_svc_t * newSvc = malloc (sizeof (rd_svc_t)), *i;
    unsigned long rnum;

    newSvc->name = strdup (name);
    while (rd_svc_find_id (RD.services, &rnum))
        rnum = rand ();
    newSvc->id = rnum;

    HASH_ADD_INT (RD.services, id, newSvc);

    return 0;
}

void destroy_property (rd_property_t * delProperty)
{
    if (delProperty->value.type == STRING)
        free (delProperty->value.property_value_u_u.s);
    free (delProperty);
}

void destroy_properties_list (rd_property_t * box)
{
    rd_property_t * i_tmp, *i_iter;
    HASH_ITER (hh, box, i_iter, i_tmp)
    {
        destroy_property (i_iter);
        HASH_DEL (box, i_iter);
    }
}

void destroy_instance (rd_svc_instance_t * delInstance)
{
    destroy_properties_list (delInstance->properties);
    free (delInstance->name);
    free (delInstance);
}

void destroy_svc (rd_svc_t * delSvc)
{
    rd_svc_instance_t * i_tmp, *i_iter;
    HASH_ITER (hh, delSvc->instances, i_iter, i_tmp)
    {
        destroy_instance (i_iter);
    }
    destroy_properties_list (delSvc->properties);
    HASH_CLEAR (hh, delSvc->instances);
    free (delSvc->name);
    free (delSvc);
}

int delete_svc (svc_id_t id)
{
    rd_svc_t * delSvc = rd_svc_find_id (RD.services, &id);

    if (!delSvc)
        return 1;

    destroy_svc (delSvc);
    HASH_DEL (RD.services, delSvc);
}
