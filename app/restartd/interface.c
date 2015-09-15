#include "manager.h"
#include "restartd_rpc.h"

void * instance_start_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    note_send (MSG_START, id, i_id, 0);
    return &result;
}

void * instance_stop_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    note_send (MSG_STOP, id, i_id, 0);
    return &result;
}

void * instance_reload_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    note_send (MSG_RELOAD, id, i_id, 0);
    return &result;
}

void * instance_restart_1_svc (svc_id_t id, svc_id_t i_id, struct svc_req * req)
{
    static int result;
    note_send (MSG_RESTART, id, i_id, 0);
    return &result;
}

int * message_available_1_svc (char * msg, struct svc_req * req)
{
    static int result;
    return &result;
}

void * message_1_svc (opaque_array msg, struct svc_req * req)
{
    static opaque_array result;
    return &result;
}
