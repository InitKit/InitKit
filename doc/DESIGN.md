# System XVI: design and architecture

![System XVI Logo](logotype.png)

### Overview
System XVI consists of several components which interoperate using the *TI-RPC*
protocol. This stands in contrast to the integrative model that has grown in
popularity in recent years. This document aims to provide a high-level overview
of these components and how they interact.

These components have defined interfaces shown in the *RPCGen* interface 
descriptions (found in **.x** files). It is an explicit goal of S16 to make
these interfaces simple and thus easily reimplementable. Indeed, this plays an
important role in the concept of *Restarter Delegates*, which will be explained
herein.

### Components

#### s16.repositoryd
This is the *Service Configuration Repository*. It is an in-memory store of
services, which contain instances and property lists, and instances, which
contain property lists. Instances and services additionally have a name field.

This component is perhaps the most central to S16. It is the link that connects
other components to one-another. It exports an interface that allows for the
insertion, modification, and deletion of services, their instances, and their
properties. 
 
#### s16.restartd
This is the *Master Service Restarter*. It is the default restarter for 
services; it implements a supervision strategy that resembles that of SystemD.
This model is well-suited to S16: it is flexible, and it - like SMF before it -
overcomes the shortcoming of traditional daemon management as found in SysVInit
and many other systems, wherein a service may fork off processes and escape
from tracking - attempting to shut down a service may leave traces of it
running.

This is accomplished by using the *libs16rr* (*S16 Restarter Routines*) library.
This library implements a simple abstraction atop a system's native process
monitoring functionality - on Linux, the cn_proc netlink connector, and on BSD,
the EVFILT_PROC KEvent filter. By inspecting the forking and exiting of a
service, we are able to ensure it cannot escape from supervision.

A service has, in the form of properties, a set of *methods*. A method is that
which performs an action for a service - for example, the prestart method is
executed before a service's start method is executed, which starts the service
proper. There may be a stop method, which overrides the typical behaviour for
terminating a service with some other behaviour - perhaps sending the service
a message through some IPC protocol telling it to clean up and exit.

#### svccfg
This is the *Service Configuration Utility*. It is responsible for installing
services into the configuration repository. It is capable of parsing both S16
native service unit files, and SystemD unit files (note: this is not yet
functional).

Once such a service has been parsed, svccfg can install it into the
configuration repository. svccfg is also responsible for finding the initial
list of services on a system and installing these into the repository.

#### s16.svcorder
This is the *Service Ordering Manager*. It determines the appropriate order
in which services should be started and decides upon a plan of action to bring
up a system.

#### Restarter delegates
These form the primary way through which S16 may be extended. A restarter
delegate implements a supervision strategy for a particular type of service.

For example, it would be quite possible to create an inetd variation which
acts as an S16 delegate restarter. It would convert primitive management
directives, for example, _start sshd_, into the appropriate action - for an
inetd form, it would start listening for connections on the appropriate port,
and launch instances of the _sshd_ service to handle incoming connections.

### Interactions

#### How they will interact on a typical boot

##### Init
Init has a simple job. It should reap zombies, be ready for shutdown, and
keep s16.restartd running.

##### s16.restartd
s16.restard will attempt to connect to repositoryd via RPC. This will fail on
boot. It will load a hard-coded repositoryd service and run it. When
repositoryd is up, it will establish a connection. It will update repositoryd
with service state and property changes.

##### s16.repositoryd
s16.repositoryd will run and launch svccfg.

As events occurs (changes of service state or properties), repositoryd will
deliver to processes that have subscribed for notification information about
these changes.

##### svccfg
svccfg will scan for services in the appropriate folder. svccfg will deliver to
repositoryd via RPC the serialised representation of these services. 
svccfg will now tell restartd to launch the s16.svcorder service.

##### s16.svcorder
s16.svcorder will scan the dependencies of the services in the default
milestone and will ask s16.restartd to launch these services in the right
order. svcorder will subscribe to repositoryd service state notifications
in order to take the appropriate further action in starting services. It will
remain running in order to continue to perform the correct actions when a
service should stop, as well as to handle requests to shut down by ordering
the shutdown milestone.