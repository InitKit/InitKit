#include "manager.h"
#include "config-subscriber_rpc.h"

int * config_service_installed_1_svc (rpc_svc_t rsvc, struct svc_req * req)
{
    static int result = 0;
    printf ("Service installed\n");
    return &result;
}

int * config_instance_state_1_svc (svc_id_t id, svc_id_t i_id,
                                   svc_state_e state, struct svc_req * req)
{
    static int result = 0;
    return &result;
}