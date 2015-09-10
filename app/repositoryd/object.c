#include <stdlib.h>
#include <string.h>
#include "s16.h"
#include "s16db.h"
#include "internal.h"

#define FindSvcOrReturn(list, id)                                              \
    svc_t * Svc = svc_find_id (list, id);                                      \
    if (!Svc)                                                                  \
        return 1;

#define DestroySvcIfExists(list, name)                                         \
    svc_t * Svc = svc_find_name (list, name);                                  \
    if (Svc)                                                                   \
    {                                                                          \
        List_del (list, Svc);                                                  \
        rnum = Svc->id;                                                        \
        destroy_svc (Svc);                                                     \
    }

svc_id_t insert_svc (char const * name)
{
    unsigned long rnum;
    DestroySvcIfExists (RD.services, name);
    svc_t * newSvc = s16_svc_new ();

    newSvc->name = strdup (name);
    while (svc_find_id (RD.services, rnum))
        rnum = rand ();
    newSvc->id = rnum;

    svc_list_add (RD.services, newSvc);

    return rnum;
}

svc_id_t install_svc (svc_t * svc)
{
    unsigned long rnum;
    DestroySvcIfExists (RD.services, svc->name);

    while (svc_find_id (RD.services, rnum))
        rnum = rand ();
    svc->id = rnum;

    svc_list_add (RD.services, svc);
    return rnum;
}

int delete_svc (svc_id_t id)
{
    FindSvcOrReturn (RD.services, id);

    svc_list_del (RD.services, Svc);
    destroy_svc (Svc);
    return 0;
}

int count_svcs () { return List_count (RD.services); }

svc_list retrieve_all_svcs () { return RD.services; }

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
