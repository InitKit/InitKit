# Design
Some thoughts on designing a new service manager.

### PID 0
Can and should be as minimal as possible.
If we need smartness, we can do it in another process, like
we've always done ever since Ken and Dennis invented UNIX.

### Supervising services
Yes, we can do this properly and minimalistically.
We can use something like libkqueue to supervise a process,
and to detect when it forks or ends. We can add intelligent
behaviour on what to do when a process does something in a 
clean and modular way.

### SystemD unit file converter
We should do this because we can, and because it will provide
a migration path from systemd to us.


### Sources for inspiration
We should be certain to look at other systems to learn from
where they succeed and where they fail.

Some of them include:
 * SMF
 * SystemD
 * Upstart
 * Daemontools/runit
 * NOSH
 * launchd
