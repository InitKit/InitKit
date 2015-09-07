#include <climits>
#include <strings.h>
#include <unistd.h>
#include "manager.h"

#define CompareType(typ)                                                       \
    !strcasecmp (svc_object_get_property_string (svc, "Service.Type"), typ)

SvcManager::SvcManager (SystemDr & sd, svc_t * svc)
    : m_svc (svc), m_state_factory (svc, *this), m_sd (sd)
{
    if (CompareType ("simple"))
        m_type = SIMPLE;
    else if (CompareType ("forking"))
        m_type = FORKING;
    else
        printf ("Fail: service type unknown\n");

    timeout_start = 90;
    timeout_stop = 90;
}

void SvcManager::register_pid (pid_t pid)
{
    pt_watch_pid (m_sd.m_ptrack, pid);
    m_pids.push_back (pid);
}

void SvcManager::deregister_pid (pid_t pid)
{
    for (std::vector<pid_t>::iterator it = m_pids.begin (); it != m_pids.end ();
         it++)
    {
        if (*it == pid)
        {
            m_pids.erase (it);
            break;
        }
    }
    pt_disregard_pid (m_sd.m_ptrack, pid);
}

void SvcManager::launch ()
{
    if (m_state_stack.size () > 0)
        return;
    if (svc_object_get_property_string (m_svc, "Service.ExecStartPre"))
        m_state_stack.push_back (m_state_factory.new_start_pre ());
}

int SvcManager::fork_register_exec (const char * cmd_)
{
    int n_spaces = 0, ret = 1;
    char * cmd = strdup (cmd_), * tofree = cmd, ** argv = NULL;
    pid_t newPid;

    strtok (cmd, " ");

    while (cmd)
    {
        argv = (char **)realloc (argv, sizeof (char *) * ++n_spaces);
        argv[n_spaces - 1] = cmd;
        cmd = strtok (NULL, " ");
    }

    argv = (char **)realloc (argv, sizeof (char *) * (n_spaces + 1));
    argv[n_spaces] = 0;

    newPid = fork ();

    if (newPid == 0) /* child */
    {
        execvp (argv[0], argv);
    }
    else if (newPid < 0) /* fail */
    {
        printf ("FAILED TO FORK\n");
    }
    else /* parent */
    {
        ret = 0;
        register_pid (newPid);
    }

    free (argv);
    free (tofree);

    return ret;
}
