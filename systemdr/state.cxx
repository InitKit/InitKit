#include "state.h"
#include "manager.h"

StartPreState::StartPreState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    manager.fork_register_exec (
        svc_object_get_property_string (svc, "Service.ExecStartPre"));
}
