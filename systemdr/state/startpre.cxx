#include "state.h"
#include "manager.h"

StartPreState::StartPreState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    fprintf (stderr, "Service enters StartPre\n");
    /* Later, this should set an Instance ID instead, which could be obtained
     * from the configuration repository. */
    m_failed = false;
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
        m_manager.main_pid = 0;
        /* SystemD will declare failure if the main PID amongst an execstartpre
         * should exit badly.
         * We set a "fail" flag and move to maintenance state after
         * terminating remaining PIDs if any non-'-'-prefixed ExecStartPre
         * fails. */
        if (exit_was_abnormal (info->flags))
            m_failed = true;

        if (m_manager.m_pids.size ())
        {
            m_manager.push_state (
                m_manager.m_state_factory.new_state<StopTermState> ());
        }
    }
}

int StartPreState::loop_iteration ()
{
    if (!m_manager.m_pids.size ())
    {
        fprintf (stderr, "No PIDs left\n");
        m_manager.sd.deregister_timer (m_timer);
        m_timer = 0;

        if (m_failed == true && (m_manager.m_rtype == RESTART_NEVER ||
                                 m_manager.m_rtype == RESTART_ON_SUCCESS))
            m_manager.replace_state (
                m_manager.m_state_factory.new_state<MaintenanceState> ());
        /* transfer to next execstartpre instead some day, here */
        else if (m_manager.m_type == SIMPLE)
        {
            SvcManager & mgr = m_manager;
            mgr.replace_state (mgr.m_state_factory.new_state<RunState> ());
        }
    }
}

bool StartPreState::timer_cb (unsigned int t)
{
    printf ("Timeout! (StartPreState)\n");
    m_failed = true;
    m_manager.push_state (
        m_manager.m_state_factory.new_state<StopTermState> ());
}
