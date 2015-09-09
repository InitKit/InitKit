#ifndef SD_DEFS_H__
#define SD_DEFS_H__

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

enum SvcRestartTypes
{
    RESTART_NEVER,
    RESTART_ON_SUCCESS,
    RESTART_ON_FAILURE,
    RESTART_ALWAYS
};

#endif
