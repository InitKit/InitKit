#include "manager.h"
#include "restartd_rpc.h"

int * instance_start_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    return &result;
}

int * instance_stop_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    return &result;
}

int * instance_reload_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    return &result;
}

int * instance_restart_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    return &result;
}

int * message_available_1_svc (char * msg, struct svc_req * req)
{
    static int result;
    return &result;
}

opaque_array * message_1_svc (opaque_array msg, struct svc_req * req)
{
    static opaque_array result;
    return &result;
}
