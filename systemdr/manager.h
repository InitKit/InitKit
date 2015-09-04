#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sys/event.h>
#include <sys/wait.h>
#include <vector>

#include "s16.h"
#include "s16db.h"
#include "state.h"

class SvcManager
{
    std::vector<std::pair<pid_t, pid_t> > m_pid_stack;
    std::vector<std::shared_ptr<SvcState> > m_state_stack;
    svc_t * m_svc;

  public:
    SvcManager (svc_t * svc) : m_svc (svc) {}
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
