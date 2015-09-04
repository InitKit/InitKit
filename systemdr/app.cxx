#include <signal.h>

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

    sa.sa_flags = 0;
    sigemptyset (&sa.sa_mask);
    sa.sa_handler = discard_signal;
    sigaction (SIGCHLD, &sa, NULL);
}

void SystemDr::main_loop () {}
