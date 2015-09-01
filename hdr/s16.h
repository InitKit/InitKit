#ifndef _S16_H_
#define _S16_H_

#include "uthash.h"
#include "repositoryd_rpc.h"

typedef struct svc_s
{
    unsigned long id;
    char * name;
    struct property_s * properties;
    struct svc_instance_s * instances;

    UT_hash_handle hh;
} svc_t;

typedef struct svc_instance_s
{
    unsigned int id;
    char * name;
    struct property_s * properties;

    unsigned int svc_id;
    UT_hash_handle hh;
} svc_instance_t;

typedef struct property_s
{
    unsigned int id; /* uthash key */
    char * name;     /* key, but not uthash key */
    pval_t value;

    property_parent_type_t parent_type;
    unsigned int parent_id;
    UT_hash_handle hh;
} property_t;

#endif
