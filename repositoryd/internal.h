#ifndef __INTERNAL_H_
#define __INTERNAL_H_

#include "storage.h"

typedef struct repositoryd_s
{
    svc_t * services;
} repositoryd_t;

extern repositoryd_t RD;

#endif
