#include "state.h"
#include "manager.h"

StartPreState::StartPreState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    /* Later, this should set an Instance ID instead, which could be obtained
     * from the configuration repository. */
    svc_object_set_property_string (svc, "S16.Status", "initialising");
    m_manager.main_pid = manager.fork_register_exec (
        svc_object_get_property_string (svc, "Service.ExecStartPre"));
    m_timer = manager.sd.register_timer (
        manager.timeout_start,
        std::bind (&StartPreState::timer_cb, this, std::placeholders::_1));
}

int StartPreState::process_event (pt_info_t * info)
{
    if (info->event == PT_EXIT && info->pid == m_manager.main_pid)
    {
        fprintf (stderr, "Main PID exited\n");
    }
}

bool StartPreState::timer_cb (unsigned int t) { printf ("Timer triggered\n"); }
