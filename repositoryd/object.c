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

gen_find_id_wrapper (svc);
gen_find_name_wrapper (svc);

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
