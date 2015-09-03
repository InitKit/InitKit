#ifndef __STATE_H__
#define __STATE_H__

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

class SvcState
{
    class Manager & m_manager;

  public:
    virtual int process ();
};

#endif
