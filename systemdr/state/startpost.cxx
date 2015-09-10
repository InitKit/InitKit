#include "state.h"
#include "manager.h"

StartPostState::StartPostState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    m_failed = false;
    m_main_pid = manager.fork_register_exec (
        svc_object_get_property_string (svc, "Service.ExecStartPost"));
    if (!m_main_pid)
        m_failed = true;
    else
        m_pids.push_back (m_main_pid);
}

int StartPostState::process_event (pt_info_t * info)
{
    if (info->event == PT_CHILD)
    {
        bool rel;
        for (const pid_t pid : m_pids)
        {
            if (pid == info->ppid)
            {
                m_pids.push_back (info->pid);
            }
        }

        if (m_manager.m_pids.size ())
        {
            fprintf (stderr, "Pushing StopTerm to kill remaining PIDs\n");
        }
    }
    if (info->event == PT_EXIT && info->pid == m_main_pid)
    {
        fprintf (stderr, "Main StartPost PID exited\n");

        if (m_manager.m_pids.size ())
        {
            fprintf (stderr, "Pushing StopTerm to kill remaining PIDs\n");
        }
    }
}

int StartPostState::loop_iteration ()
{
    if (!m_manager.m_pids.size ())
    {
        fprintf (stderr, "No PIDs left\n");
    }
}
