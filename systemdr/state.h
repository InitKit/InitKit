#ifndef __STATE_H__
#define __STATE_H__

#include <cstdio>
#include <memory>
#include "s16.h"
#include "s16rr.h"

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
    virtual int process_event (pt_info_t *) = 0;
};

class StopTermState : public SvcState
{
    unsigned int m_timer;

  public:
    StopTermState (svc_t * svc, SvcManager & manager);
    ~StopTermState () {}
    int loop_iteration () {}
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t);
};

class StartPreState : public SvcState
{
    unsigned int m_timer;

  public:
    StartPreState (svc_t * svc, SvcManager & manager);
    ~StartPreState () {}
    int loop_iteration () {}
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t);
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
    std::shared_ptr<StopTermState> new_stop_term ()
    {
        return std::shared_ptr<StopTermState> (
            new StopTermState (m_svc, m_manager));
    }
};

#endif
