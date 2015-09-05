#include <errno.h>
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

void SystemDr::main_loop ()
{
    int i;
    struct kevent ev;
    struct timespec tmout = {0, /* return at once initially */
                             0};
    while (1)
    {
        memset (&ev, 0x00, sizeof (struct kevent));

        i = kevent (m_kq, NULL, 0, &ev, 1, &tmout);
        if (i == -1)
            if (errno == EINTR)
                continue;
            else
                fprintf (stderr, "Error: i = -1\n");

        tmout.tv_sec = 3;

        switch (ev.filter)
        {
        case EVFILT_SIGNAL:
            printf ("Signal received: %d. Additional data: %d\n", ev.ident,
                    ev.data);
            break;
        }

        for (SvcManager & svc : m_managers)
        {
            svc.launch ();
        }
    }
}
