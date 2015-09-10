#include <sys/types.h>
#include <signal.h>
#include "state.h"
#include "manager.h"

StopTermState::StopTermState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    fprintf (stderr, "Service enters StopTerm\n");
    m_timer = manager.sd.register_timer (
        manager.timeout_stop,
        std::bind (&StopTermState::timer_cb, this, std::placeholders::_1));

    for (const pid_t pid : manager.m_pids)
    {
        kill (pid, SIGTERM);
    }
}

int StopTermState::process_event (pt_info_t *)
{
    if (m_manager.m_pids.size () == 0)
    {
        fprintf (stderr, "PIDs purged\n");
        m_manager.pop_state ();
    }
}

bool StopTermState::timer_cb (unsigned int t) { printf ("Timer triggered\n"); }
