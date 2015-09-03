#ifndef _S16_DB_H_
#define _S16_DB_H_

#include <string.h>
#include "s16.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SVC_IS_NULL(svc) !strncmp (svc->name, "NULL", 4)

CLIENT * s16db_context_create ();
void s16db_context_destroy (CLIENT * clnt);

svc_id_t s16db_svc_insert (CLIENT * clnt, char const * name);
svc_id_t s16db_svc_install (CLIENT * clnt, svc_t * svc);
int s16db_svc_delete (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve_by_name (CLIENT * clnt, char * name);
svc_list s16db_svc_retrieve_all (CLIENT * clnt);
int s16db_svc_set_property_int (CLIENT * clnt, svc_id_t id, char const * key,
                                long value);
int s16db_svc_set_property_string (CLIENT * clnt, svc_id_t id, char const * key,
                                   char const * value);

#ifdef __cplusplus
}
#endif

#endif
