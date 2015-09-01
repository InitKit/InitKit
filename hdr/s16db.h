#ifndef _S16_DB_H_
#define _S16_DB_H_

#include "s16.h"

#define gen_find_id_wrapper_proto(type)                                        \
    type##_t * type##_find_id (type##_t * box, unsigned long * keyptr);

#define gen_find_name_wrapper_proto(type)                                      \
    type##_t * type##_find_name (type##_t * box, char * name);

gen_find_id_wrapper_proto (svc);
gen_find_name_wrapper_proto (svc);

void destroy_property (property_t * delProperty);
void destroy_properties_list (property_t * box);
void destroy_instance (svc_instance_t * delInstance);
void destroy_svc (svc_t * delSvc);

#endif
