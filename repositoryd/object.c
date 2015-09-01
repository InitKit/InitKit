#include "s16.h"
#include "s16db.h"
#include "internal.h"

int insert_svc (char const * name)
{
    svc_t * newSvc = malloc (sizeof (svc_t)), *i;
    unsigned long rnum;

    newSvc->name = strdup (name);
    while (svc_find_id (RD.services, &rnum))
        rnum = rand ();
    newSvc->id = rnum;

    HASH_ADD_INT (RD.services, id, newSvc);

    return 0;
}

int delete_svc (svc_id_t id)
{
    svc_t * delSvc = svc_find_id (RD.services, &id);

    if (!delSvc)
        return 1;

    destroy_svc (delSvc);
    HASH_DEL (RD.services, delSvc);
}
