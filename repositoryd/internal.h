#ifndef __INTERNAL_H_
#define __INTERNAL_H_

#include "storage.h"

typedef struct repositoryd_s
{
    svc_t * services;
} repositoryd_t;

extern repositoryd_t RD;

#define gen_find_id_wrapper_proto(type)                                        \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr);

#define gen_find_name_wrapper_proto(type)                                      \
    type##_t * type##_find_name (type##_t * box, char * name);

gen_find_id_wrapper_proto (svc);
gen_find_name_wrapper_proto (svc);

#endif
