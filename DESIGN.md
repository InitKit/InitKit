# Design
Some thoughts on designing a new service manager.

### PID 0
I've borrowed Lennart's parlance for PID 1 for the banter.
PID 0 can and should be as minimal as possible.
If we need smartness, we can do it in another process, like
we've always done ever since Ken and Dennis invented UNIX.

### Libsvc
libsvc incorporates reusable components of the 'stack.'
A C structure defining a service exists here, along with an interface
for communicating with the service manager and delegated restarters.
libsvc also handles all global configuration and is the gateway to
frameworks like that which maps service structures to the appropriate
restarter.

### The service manager
The service manager does what it says. This is where dependency
ordering is found. It incorporates logic for some simple service
restarters; it transparently delegates service restarting to the
appropriate delegated restarter, whether that be in the same PID
or out thereof.

### Delegated restarters
Handle the restarting of a service unit.

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
