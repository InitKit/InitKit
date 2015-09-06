#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sys/types.h>
extern "C" {
#include <sys/event.h>
}
#include <sys/wait.h>
#include <vector>

#include "s16.h"
#include "s16db.h"
#include "s16rr.h"
#include "state.h"

class SvcManager
{
    SvcStateFactory m_state_factory;
    std::vector<pid_t> m_pids;
    std::vector<std::shared_ptr<SvcState> > m_state_stack;
    svc_t * m_svc;
    SvcTypes m_type;

  public:
    int fork_register_exec (const char * exe);
    void launch ();
    void register_pid (pid_t pid) { m_pids.push_back (pid); }
    void deregister_pid (pid_t pid)
    {
        for (std::vector<pid_t>::iterator it = m_pids.begin ();
             it != m_pids.end (); it++)
        {
            if (*it == pid)
            {
                m_pids.erase (it);
                break;
            }
        }
    }
    int pids_relevant (pid_t one, pid_t two)
    {
        for (std::vector<pid_t>::iterator it = m_pids.begin ();
             it != m_pids.end (); it++)
        {
            if (*it == one | *it == two)
            {
                return 1;
            }
        }
        return 0;
    }
    void process_event (pt_info_t *) {}
    SvcManager (svc_t * svc);
};

class SystemDr
{
    friend class SvcManager;
    std::vector<SvcManager> m_managers;
    CLIENT * m_clnt;
    int m_kq;

  public:
    SystemDr (CLIENT * clnt);
    void main_loop ();
    void add_svc (svc_t * svc) { m_managers.push_back (SvcManager (svc)); }
};

#endif
