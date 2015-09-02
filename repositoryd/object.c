#include "s16.h"
#include "s16db.h"
#include "internal.h"

#define FindSvcOrReturn(list, id)                                              \
    svc_t * Svc = svc_find_id (list, &id);                                     \
    if (!Svc)                                                                  \
        return 1;

#define DestroySvcIfExists(list, name)                                         \
    svc_t * Svc = svc_find_name (list, name);                                  \
    if (Svc)                                                                   \
    {                                                                          \
        HASH_DEL (list, Svc);                                                  \
        destroy_svc (Svc);                                                     \
    }

int insert_svc (char const * name)
{
    DestroySvcIfExists (RD.services, name);
    svc_t * newSvc = calloc (1, sizeof (svc_t)), *i;
    unsigned long rnum;

    newSvc->name = strdup (name);
    while (svc_find_id (RD.services, &rnum))
        rnum = rand ();
    newSvc->id = rnum;

    HASH_ADD_INT (RD.services, id, newSvc);

    return rnum;
}

int delete_svc (svc_id_t id)
{
    FindSvcOrReturn (RD.services, id);

    destroy_svc (Svc);
    HASH_DEL (RD.services, Svc);
}

int count_svcs () { return HASH_COUNT (RD.services); }

svc_t * retrieve_all_svcs () { return RD.services; }

int set_svc_property_int (svc_id_t id, char const * name, long value)
{
    FindSvcOrReturn (RD.services, id);
    DestroyPropIfExists (Svc->properties, name);
    unsigned long rnum;
    svc_object_set_property_int (Svc, name, value);
    return 0;
}

int set_svc_property_string (svc_id_t id, char const * name, char const * value)
{
    FindSvcOrReturn (RD.services, id);
    DestroyPropIfExists (Svc->properties, name);
    unsigned long rnum;
    svc_object_set_property_string (Svc, name, value);
    return 0;
}
