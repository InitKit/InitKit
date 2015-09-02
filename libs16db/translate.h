#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_

#include "s16.h"

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
