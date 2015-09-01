#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "uthash.h"

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
    char * name;
    unsigned int id;

    unsigned int svc_id;
    UT_hash_handle hh;
} svc_instance_t;

typedef struct property_s
{
    unsigned int id;
    enum
    {
        NUMBER,
        STRING
    } type;
    union
    {
        int i;
        char * s;
    };

    enum
    {
        SVC,
        SVCI,
        PROP,
    } parent_type;
    unsigned int parent_id;
    UT_hash_handle hh;
} property_t;

#endif
