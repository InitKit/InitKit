#include <limits.h>
#include <stdlib.h>

#include "manager.h"
#include "timer.h"

Timer * timer_find (long id)
{
    for (timer_list_iterator it = timer_list_begin (Manager.timers); it != NULL;
         timer_list_iterator_next (&it))
    {
        if (it->val->id == id)
        {
            return it->val;
        }
    }
}

void timer_del (long id)
{
    Timer * toDel;
    struct kevent ev;

    if ((toDel = timer_find (id)))
    {
        timer_list_del (Manager.timers, toDel);
        EV_SET (&ev, toDel->id, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
        kevent (Manager.kq, &ev, 1, NULL, 0, NULL);
        free (toDel);
    }
}

long timer_add (int sec, void * userData, void (*cb) (void *, long))
{
    struct kevent ev;
    long ident = rand () % UINT_MAX;
    Timer * newTimer;
    int i;

    while (timer_find (ident))
        ident = rand () % UINT_MAX;

    EV_SET (&ev, ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, sec * 1000, 0);
    i = kevent (Manager.kq, &ev, 1, NULL, 0, NULL);

    if (i == -1)
    {
        perror ("timer kevent! (add)");
        return 0;
    }

    newTimer = malloc (sizeof (Timer));
    newTimer->id = ident;
    newTimer->userData = userData;
    newTimer->cb = cb;

    timer_list_add (Manager.timers, newTimer);
    return ident;
}
