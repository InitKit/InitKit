#include <sys/types.h>
#include <unistd.h>

#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/procctl.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <kvm.h>
#elif defined(__linux__)
#include <proc/readproc.h>
#include <sys/prctl.h>
#endif

int subreap_acquire ()
{
#if defined(__FreeBSD__) || defined(__DragonFly__)
    return procctl (P_PID, getpid (), PROC_REAP_ACQUIRE, NULL);
#elif defined(__linux__)
    return prctl (PR_SET_CHILD_SUBREAPER, 1);
#endif
}

int subreap_relinquish ()
{
#if defined(__FreeBSD__) || defined(__DragonFly__)
    return procctl (P_PID, getpid (), PROC_REAP_RELEASE, NULL);
#elif defined(__linux__)
    return prctl (PR_SET_CHILD_SUBREAPER, 0);
#endif
}

int subreap_status ()
{
#if defined(__FreeBSD__) || defined(__DragonFly__)
    struct procctl_reaper_status pctl;
    procctl (P_PID, getpid (), PROC_REAP_STATUS, &pctl);
    return (pctl.rs_flags & REAPER_STATUS_OWNED);
#elif defined(__linux__)
    int status;
    prctl (PR_GET_CHILD_SUBREAPER, &status);
    return status;
#endif
}

pid_t process_get_ppid (pid_t pid)
{
#if defined(__FreeBSD__) || defined(__DragonFly__)
    struct kinfo_proc info;
    size_t l = sizeof (info);
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};
    if (sysctl (mib, 4, &info, &l, NULL, 0) < 0)
        return 0;
    return info.ki_ppid;
#elif defined(__linux__)
    proc_t info;
    get_proc_stats (pid, &info);
    return info.ppid;
#endif
}

void discard_signal (int no) { no = no; }
