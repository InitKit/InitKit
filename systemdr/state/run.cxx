#include <sys/types.h>
#include <signal.h>
#include "state.h"
#include "manager.h"

RunState::RunState (svc_t * svc, SvcManager & manager) : SvcState (svc, manager)
{
    m_failed = false;
    m_manager.main_pid = manager.fork_register_exec (
        svc_object_get_property_string (svc, "Service.ExecStart"));
    if (m_manager.main_pid)
        svc_object_set_property_string (svc, "S16.Status", "online");
}

int RunState::process_event (pt_info_t * info)
{
    if (info->event == PT_EXIT && info->pid == m_manager.main_pid)
    {
        fprintf (stderr, "Main PID exited\n");
        m_manager.main_pid = 0;

        if (m_manager.m_pids.size ())
        {
            fprintf (stderr, "Pushing StopTerm to kill remaining PIDs\n");
            m_manager.push_state (
                m_manager.m_state_factory.new_state<StopTermState> ());
        }
    }
}

int RunState::loop_iteration ()
{
    if (!m_manager.m_pids.size ())
    {
        fprintf (stderr, "No PIDs left\n");
    }
}
