#ifndef _S16_DB_H_
#define _S16_DB_H_

#include <string.h>
#include "s16.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SVC_IS_NULL(svc) (!svc || !strncmp (svc->name, "NULL", 4))

CLIENT * s16db_context_create ();
void s16db_context_destroy (CLIENT * clnt);

svc_id_t * s16db_svc_insert (CLIENT * clnt, char const * name);
svc_id_t * s16db_svc_install (CLIENT * clnt, svc_t * svc);
int * s16db_svc_delete (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve_by_name (CLIENT * clnt, char * name);
svc_list s16db_svc_retrieve_all (CLIENT * clnt);
int * s16db_svc_set_property_int (CLIENT * clnt, svc_id_t id, char const * key,
                                  long value);
int * s16db_svc_set_property_string (CLIENT * clnt, svc_id_t id,
                                     char const * key, char const * value);

rpc_property_t property_to_rpc_property (property_t * prop);
rpc_property_t * property_list_to_rpc_property_array (prop_list box);
rpc_svc_instance_t svc_instance_to_rpc_svc_instance (svc_instance_t * inst);
rpc_svc_t svc_to_rpc_svc (svc_t * svc);
rpc_svc_t * svc_list_to_rpc_svc_array (svc_list box);

property_t * rpc_property_to_property (rpc_property_t * rprop);
prop_list rpc_property_array_to_property_list (rpc_property_t rplist[],
                                               unsigned int length);
svc_instance_t * rpc_svc_instance_to_svc_instance (rpc_svc_instance_t * rinst);
svc_t * rpc_svc_to_svc (rpc_svc_t * rsvc);
svc_list rpc_svc_array_to_svc_list (rpc_svc_array_t * svcArray);

#ifdef __cplusplus
}
#endif

#endif
