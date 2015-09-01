#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "uthash.h"
#include "repositoryd_rpc.h"

typedef struct rd_svc_s
{
    unsigned long id;
    char * name;
    struct rd_property_s * properties;
    struct rd_svc_instance_s * instances;

    UT_hash_handle hh;
} rd_svc_t;

typedef struct rd_svc_instance_s
{
    unsigned int id;
    char * name;
    struct rd_property_s * properties;

    unsigned int svc_id;
    UT_hash_handle hh;
} rd_svc_instance_t;

typedef struct rd_property_s
{
    unsigned int id; /* uthash key */
    char * name;     /* key, but not uthash key */
    property_value_t value;

    property_parent_type_t parent_type;
    unsigned int parent_id;
    UT_hash_handle hh;
} rd_property_t;

#endif
