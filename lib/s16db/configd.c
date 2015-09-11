/* RestartD needs a configuration repository, but during system initialisation,
 * ConfigD is not running, as RestartD is the 2nd daemon launched after init.
 * As such, the appropriate structures for RestartD to monitor a ConfigD are
 * constructed here and inserted into the restarter. */
