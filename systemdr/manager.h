#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <sys/wait.h>
#include <memory>
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
};

#endif
