#ifndef __INTERNAL_H_
#define __INTERNAL_H_

#include "storage.h"

typedef struct repositoryd_s
{
    rd_svc_t * services;
} repositoryd_t;

extern repositoryd_t RD;

#define gen_find_id_wrapper_proto(type)                                        \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr);

#define gen_find_name_wrapper_proto(type)                                      \
    type##_t * type##_find_name (type##_t * box, char * name);

gen_find_id_wrapper_proto (rd_svc);
gen_find_name_wrapper_proto (rd_svc);

int insert_svc (char const * name);
int delete_svc (svc_id_t id);

#endif
