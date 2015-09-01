#ifndef _S16_DB_H_
#define _S16_DB_H_

#include "s16.h"

#define gen_find_id_wrapper_proto(type)                                        \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr);

#define gen_find_name_wrapper_proto(type)                                      \
    type##_t * type##_find_name (type##_t * box, char * name);

gen_find_id_wrapper_proto (svc);
gen_find_name_wrapper_proto (svc);

CLIENT * s16db_context_create ();
void s16db_context_destroy (CLIENT * clnt);

void destroy_property (property_t * delProperty);
void destroy_properties_list (property_t * box);
void destroy_instance (svc_instance_t * delInstance);
void destroy_svc (svc_t * delSvc);

/* used internally */
rpc_property_t property_to_rpc_property (property_t * prop);
rpc_property_t * property_list_to_rpc_property_array (property_t * box);
rpc_svc_instance_t svc_instance_to_rpc_svc_instance (svc_instance_t * inst);
rpc_svc_t * svc_to_rpc_svc (svc_t * svc);

#endif
