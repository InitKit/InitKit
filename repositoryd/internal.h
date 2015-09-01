#ifndef __INTERNAL_H_
#define __INTERNAL_H_

#include "s16.h"

typedef struct repositoryd_s
{
    svc_t * services;
} repositoryd_t;

extern repositoryd_t RD;

int insert_svc (char const * name);
int delete_svc (svc_id_t id);

#endif
