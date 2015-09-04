#ifndef __STATE_H__
#define __STATE_H__

#include "s16.h"
#include <memory>

enum SvcStates
{
    INACTIVE,
    START_PRE,
    START,
    START_POST,
    ONLINE,
    EXITED,
    STOP,
    STOP_SIGTERM,
    STOP_SIGKILL,
    STOP_POST,
    FAULTED,
    RESTART,
};

enum SvcTypes
{
    SIMPLE,
    FORKING,
};

struct ProcessEvent
{
    enum
    {
        EXIT_BAD,
        EXIT_GOOD,
        NEW,
    } type;
    pid_t ppid;
    pid_t pid;
};

class SvcState
{
  protected:
    svc_t * m_svc;
    class SvcManager & m_manager;

  public:
    SvcState (svc_t * svc, SvcManager & manager)
        : m_svc (svc), m_manager (manager)
    {
    }
    virtual ~SvcState () {}
    virtual int loop_iteration () = 0;
    virtual int process_event (ProcessEvent) = 0;
};

class StartPreState : public SvcState
{
  public:
    StartPreState (svc_t * svc, SvcManager & manager);
    ~StartPreState () {}
    int loop_iteration () {}
    int process_event (ProcessEvent) {}
};

class SvcStateFactory
{
    svc_t * m_svc;
    SvcManager & m_manager;

  public:
    SvcStateFactory (svc_t * svc, SvcManager & manager)
        : m_svc (svc), m_manager (manager)
    {
    }
    std::shared_ptr<StartPreState> new_start_pre ()
    {
        return std::shared_ptr<StartPreState> (
            new StartPreState (m_svc, m_manager));
    }
};

#endif
