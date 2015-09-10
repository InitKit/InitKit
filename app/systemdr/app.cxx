#include <climits>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "s16rr.h"
#include "manager.h"

SystemDr::SystemDr (CLIENT * clnt) : m_clnt (clnt)
{
    struct kevent sigfd;
    struct sigaction sa;

    subreap_acquire ();

    if ((m_kq = kqueue ()) == -1)
    {
        perror ("kqueue");
        exit (EXIT_FAILURE);
    }

    EV_SET (&sigfd, SIGCHLD, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);

    if (kevent (m_kq, &sigfd, 1, 0, 0, 0) == -1)
    {
        perror ("kqueue");
        exit (EXIT_FAILURE);
    }

    m_ptrack = pt_new (m_kq);

    sa.sa_flags = 0;
    sigemptyset (&sa.sa_mask);
    sa.sa_handler = discard_signal;
    sigaction (SIGCHLD, &sa, NULL);
}

SystemDr::~SystemDr ()
{
    pt_destroy (m_ptrack);
    close (m_kq);
}

TimerEntry * SystemDr::find_timer (unsigned int ident)
{
    for (TimerEntry & entry : m_timers)
    {
        if (entry.first == ident)
            return &entry;
    }
    return 0;
}

unsigned int SystemDr::register_timer (unsigned int sec,
                                       std::function<bool(unsigned int)> cb)
{
    struct kevent ev;
    int i;
    unsigned int ident = rand () % UINT_MAX;

    while (find_timer (ident))
        ident = rand () % UINT_MAX;

    EV_SET (&ev, ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, sec * 1000, 0);
    i = kevent (m_kq, &ev, 1, NULL, 0, NULL);

    if (i == -1)
    {
        fprintf (stderr, "timer kevent!\n");
        return 0;
    }
    else
    {
        TimerEntry entry;
        entry.first = ident;
        entry.second = cb;
        m_timers.push_back (entry);
        return ident;
    }
}

void SystemDr::deregister_timer (unsigned int ident)
{
    struct kevent ev;
    int i;

    EV_SET (&ev, ident, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
    i = kevent (m_kq, &ev, 1, NULL, 0, NULL);
    if (i == -1)
        fprintf (stderr, "timer kevent! (deletion)\n");

    for (std::vector<TimerEntry>::iterator it = m_timers.begin ();
         it != m_timers.end (); it++)
    {
        if (it->first == ident)
        {
            m_timers.erase (it);
            return;
        }
    }
}

void SystemDr::main_loop ()
{
    int i;
    struct kevent ev;
    struct timespec tmout = {0, /* return at once initially */
                             0};
    while (1)
    {
        pt_info_t * info;
        memset (&ev, 0x00, sizeof (struct kevent));

        i = kevent (m_kq, NULL, 0, &ev, 1, &tmout);
        if (i == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                fprintf (stderr, "Error: i = -1\n");
            }
        }

        tmout.tv_sec = 3;

        if ((info = pt_investigate_kevent (m_ptrack, &ev)))
            goto pinfo;

        switch (ev.filter)
        {
        case EVFILT_SIGNAL:
        {
            printf ("Signal received: %d. Additional data: %d\n", ev.ident,
                    ev.data);
            if (ev.ident == SIGCHLD)
                while (waitpid ((pid_t)(-1), 0, WNOHANG) > 0)
                    ;
            break;
        }

        case EVFILT_TIMER:
            TimerEntry * entry;
            if ((entry = find_timer (ev.ident)))
            {
                entry->second (ev.ident);
                deregister_timer (ev.ident);
                break;
            }
            break;
        }

        goto post_pinfo;

    pinfo:
        for (std::shared_ptr<SvcManager> & svc : m_managers)
        {
            if (svc->pids_relevant (info->pid, info->ppid))
                svc->process_event (info);
        }
        free (info);

    post_pinfo:
        for (std::shared_ptr<SvcManager> & svc : m_managers)
        {
            svc->launch ();
            svc->loop_iteration ();
        }
    }
}
