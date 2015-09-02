#ifndef _S16_H_
#define _S16_H_

#include "list.h"
#include "repositoryd_rpc.h"

typedef struct svc_s svc_t;
typedef struct svc_instance_s svc_instance_t;
typedef struct property_s property_t;

ListGenForNameType (svc, svc_t);
ListGenForNameType (inst, svc_instance_t);
ListGenForNameType (prop, property_t);

typedef struct svc_s
{
    unsigned long id;
    char * name;
    prop_list properties;
    inst_list instances;
} svc_t;

typedef struct svc_instance_s
{
    unsigned long id;
    char * name;
    prop_list properties;
    unsigned int svc_id;
} svc_instance_t;

typedef struct property_s
{
    unsigned long id; /* uthash key */
    char * name;      /* key, but not uthash key */
    pval_t value;
    property_parent_type_t parent_type;
} property_t;

#define gen_find_id_wrapper_proto(name, type)                                  \
    type * name##_find_id (name##_list box, unsigned long keyptr);

#define gen_find_name_wrapper_proto(name, type)                                \
    type * name##_find_name (name##_list box, char const * name);

gen_find_id_wrapper_proto (svc, svc_t);
gen_find_name_wrapper_proto (svc, svc_t);

gen_find_id_wrapper_proto (prop, property_t);
gen_find_name_wrapper_proto (prop, property_t);

void svc_object_set_property_string (svc_t * Svc, const char * key,
                                     const char * value);
void svc_object_set_property_int (svc_t * Svc, const char * key, long value);

void destroy_property (property_t * delProperty);
void destroy_properties_list (property_t * box);
void destroy_instance (svc_instance_t * delInstance);
void destroy_svc (svc_t * delSvc);
void destroy_svcs_list (svc_t * box);

#define DestroyPropIfExists(list, name)                                        \
    property_t * Prop = property_find_name (list, name);                       \
    if (Prop)                                                                  \
    {                                                                          \
        HASH_DEL (list, Prop);                                                 \
        destroy_property (Prop);                                               \
    }

#endif
