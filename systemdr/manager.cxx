#include <strings.h>
#include <unistd.h>
#include "manager.h"

#define CompareType(typ)                                                       \
    !strcasecmp (svc_object_get_property_string (svc, "Service.Type"), typ)

SvcManager::SvcManager (svc_t * svc) : m_svc (svc), m_state_factory (svc, *this)
{
    if (CompareType ("simple"))
        m_type = SIMPLE;
    else if (CompareType ("forking"))
        m_type = FORKING;
    else
        printf ("Fail: service type unknown\n");

    if (svc_object_get_property_string (svc, "Service.ExecStartPre"))
        m_state_stack.push_back (m_state_factory.new_start_pre ());
}

int SvcManager::fork_register_exec (const char * cmd_)
{
    int n_spaces = 0;
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
        register_pid (newPid, 0);
    }

    free (argv);
    free (tofree);
}
