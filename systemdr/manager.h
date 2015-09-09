#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <cstdio>
#include <cstdlib>
#include <functional>
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

typedef std::function<bool(unsigned int)> TimerCb;
typedef std::pair<unsigned int, TimerCb> TimerEntry;

class SvcManager
{
    std::vector<std::shared_ptr<SvcState> > m_state_stack;
    /* This is the service we're running on. */
    svc_t * m_svc;

  public:
    /* Our service-state factory. */
    SvcStateFactory m_state_factory;
    /* A reference to the System Dr. manager object. */
    class SystemDr & sd;
    /* The present primary PID we're monitoring. */
    pid_t main_pid;
    /* The vector of service PIDs. */
    std::vector<pid_t> m_pids;
    /* The start and stop timeouts. 90 by default. */
    unsigned int timeout_start, timeout_stop;
    /* The type of the service - simple, forking, etc. */
    SvcTypes m_type;
    /* The restart discipline for the service. */
    SvcRestartTypes m_rtype;
    /* Which exec states have defined things to do. */
    bool ExecStartPre, ExecStartPost, ExecStop, ExecStopPost;

    SvcManager (SystemDr & sd, svc_t * svc);

    pid_t fork_register_exec (const char * exe);
    void launch ();
    void loop_iteration () { m_state_stack.back ()->loop_iteration (); }

    void register_pid (pid_t pid);
    void deregister_pid (pid_t pid);
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

    void process_event (pt_info_t * pt)
    {
        if (pt->event == PT_CHILD)
            register_pid (pt->pid);
        else if (pt->event == PT_EXIT)
            deregister_pid (pt->pid);
        m_state_stack.back ()->process_event (pt);
    }

    void push_state (std::shared_ptr<SvcState> state)
    {
        m_state_stack.push_back (state);
    }
    void pop_state () { m_state_stack.pop_back (); }
};

class SystemDr
{
    friend class SvcManager;
    std::vector<std::shared_ptr<SvcManager> > m_managers;
    CLIENT * m_clnt;
    int m_kq;
    process_tracker_t * m_ptrack;

    /* Here, we store a pair of timer ID and the callback function. */
    std::vector<TimerEntry> m_timers;

  public:
    SystemDr (CLIENT * clnt);
    ~SystemDr ();

    /* Search for timer by ident.
    Returns a pointer to a TimerEntry in the timers vector */
    TimerEntry * find_timer (unsigned int ident);
    /* This registers a timer, and returns its identifier. */
    unsigned int register_timer (unsigned int sec,
                                 std::function<bool(unsigned int)> cb);
    /* This deregisters a timer.
     * Note that this only makes sense for a timer that hasn't
     * yet elapsed, as these are deleted after trigger. */
    void deregister_timer (unsigned int ident);

    void main_loop ();
    void add_svc (svc_t * svc)
    {
        m_managers.push_back (
            std::shared_ptr<SvcManager> (new SvcManager (*this, svc)));
    }
};

#endif
