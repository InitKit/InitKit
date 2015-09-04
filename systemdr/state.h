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
    class Manager & m_manager;

  public:
    virtual ~SvcState ();
    virtual int loop_iteration ();
    virtual int process_event (ProcessEvent);
};

#endif
