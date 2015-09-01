#include "storage.h"
#include "internal.h"

#define gen_find_int_wrapper(type)                                             \
    inline type##_t * type##_find_int (type##_t * box, unsigned long * keyptr) \
    {                                                                          \
        type##_t * i;                                                          \
        HASH_FIND_INT (box, keyptr, i);                                        \
        return i;                                                              \
    }

gen_find_int_wrapper (svc)

    int insert_svc (char const * name)
{
    svc_t * newSvc = malloc (sizeof (svc_t)), *i;
    unsigned long rnum;

    newSvc->name = strdup (name);
    while (svc_find_int (RD.services, &rnum))
        rnum = rand ();
    newSvc->id = rnum;

    HASH_ADD_INT (RD.services, id, newSvc);
}
