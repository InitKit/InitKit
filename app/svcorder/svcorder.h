#ifndef MANAGER_H_
#define MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "list.h"
#include "s16.h"

typedef struct svcorder_s
{
    int kq;
    CLIENT * clnt_cfg;
} svcorder_t;

extern svcorder_t Svcorder;

#define PORT 12284

#endif
