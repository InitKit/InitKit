#ifndef _S16_DB_H_
#define _S16_DB_H_

#include "s16.h"

CLIENT * s16db_context_create ();
void s16db_context_destroy (CLIENT * clnt);

int s16db_svc_insert (CLIENT * clnt, char const * name);
int s16db_svc_delete (CLIENT * clnt, svc_id_t id);
svc_t * s16db_svc_retrieve (CLIENT * clnt, svc_id_t id);
int s16db_svc_set_property_int (CLIENT * clnt, svc_id_t id, char const * key,
                                long value);
int s16db_svc_set_property_string (CLIENT * clnt, svc_id_t id, char const * key,
                                   char const * value);

#endif
