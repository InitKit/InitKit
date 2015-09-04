#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/wait.h>
#include <vector>

#include "s16.h"
#include "s16db.h"
#include "state.h"

/* pair: pid, ppid */
typedef std::pair<pid_t, pid_t> PidPair;

class SvcManager
{
    SvcStateFactory m_state_factory;
    std::vector<PidPair> m_pids;
    std::vector<std::shared_ptr<SvcState> > m_state_stack;
    svc_t * m_svc;
    SvcTypes m_type;

  public:
    int fork_register_exec (const char * exe);
    void register_pid (pid_t pid, pid_t ppid)
    {
        m_pids.push_back (PidPair (pid, ppid));
    }
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
