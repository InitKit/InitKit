#include "state.h"
#include "manager.h"

StartPreState::StartPreState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    manager.fork_register_exec (
        svc_object_get_property_string (svc, "Service.ExecStartPre"));
    m_timer = manager.sd.register_timer (
        manager.timeout_start,
        std::bind (&StartPreState::timer_cb, this, std::placeholders::_1));
}

bool StartPreState::timer_cb (unsigned int t) { printf ("Timer triggered\n"); }
