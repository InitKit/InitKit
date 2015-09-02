#ifndef _S16_DB_H_
#define _S16_DB_H_

#include "s16.h"

#define gen_find_id_wrapper_proto(type)                                        \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr);

#define gen_find_name_wrapper_proto(type)                                      \
    type##_t * type##_find_name (type##_t * box, char const * name);

gen_find_id_wrapper_proto (svc);
gen_find_name_wrapper_proto (svc);

gen_find_id_wrapper_proto (property);
gen_find_name_wrapper_proto (property);

void destroy_property (property_t * delProperty);
void destroy_properties_list (property_t * box);
void destroy_instance (svc_instance_t * delInstance);
void destroy_svc (svc_t * delSvc);

CLIENT * s16db_context_create ();
void s16db_context_destroy (CLIENT * clnt);

int s16db_svc_insert (CLIENT * clnt, char const * name);
int s16db_svc_delete (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve (CLIENT * clnt, svc_id_t id);
int s16db_svc_set_property_int (CLIENT * clnt, svc_id_t id, char const * key,
                                long value);
int s16db_svc_set_property_string (CLIENT * clnt, svc_id_t id, char const * key,
                                   char const * value);

/* used internally */
rpc_property_t property_to_rpc_property (property_t * prop);
rpc_property_t * property_list_to_rpc_property_array (property_t * box);
rpc_svc_instance_t svc_instance_to_rpc_svc_instance (svc_instance_t * inst);
rpc_svc_t * svc_to_rpc_svc (svc_t * svc);

property_t * rpc_property_to_property (rpc_property_t * rprop);
property_t * rpc_property_array_to_property_list (rpc_property_t * rplist[],
                                                  unsigned int length);
svc_instance_t * rpc_svc_instance_to_svc_instance (rpc_svc_instance_t * rinst);
svc_t * rpc_svc_to_svc (rpc_svc_t * rsvc);

#endif
