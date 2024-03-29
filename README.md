![InitKit](docs/Logo.png)
=========================

**InitKit** is the standard Unix init system. It is designed for flexibility in
accordance with a special operationalisation of the principles of the Unix Way.
A novel implementation of the *systemd* init system in JavaScript is hosted atop
a lightweight native framework. Relatively high fidelity is maintained to the
semantics of the systemd transaction generator and job engine.

InitKit is not intended at present to be be used in production. The code is
being written in exploratory fashion for now, and refactoring of some components
will be necessary.

Architecture
------------

The InitKit Scheduler Service, the program which implements the majority of
the InitKit system, is implemented in a modular, layered fashion. We will
start with the lowest levels of that service and proceed upwards.

## Portability Layer

This layer sits directly atop the provisions of the host operating system. The
most basic operations on which the event loop depends may be indirected through
this layer.

## Event Loop (EVLOOP)

InitKit is designed with asynchronicity in mind. A Kernel Queues (or
Libkqueue on non-BSD systems) event loop is the centrepiece of this.

## Core Services

This layer is comprised of several modules which integrate with one-another.

### Job Scheduler (JOBSCHED)

Job scheduling is more thoroughly described in the `niw_scheduling(7)` manual
page.

Responsible for scheduling jobs to run at the correct time. Jobs are
state-querying or state-change requests on objects, e.g. "Start OpenSSH".

The Transaction Generator is a major submodule of the Job Scheduler. It prepares
Transactions which contain a set of jobs to be run in pursuit of a particular
goal job, e.g. "Start OpenSSH" or "Stop NetworkManager".
Transactions are generated with respect to the relationships between objects
specified in the Object Graph.

The result of transaction generation is a set of jobs to be run. With respect
to the `before` and `after` ordering directives, a directed acyclic graph is
formed out of all the objects on which a job is present therefor in the
transaction. Transactions containing conflicting jobs, or out of which a DAG
cannot be formed, are rejected; these arise only from improper cyclic ordering
specifications or conflicting requirements between objets.

Jobs are run starting with the leaves of the graph. As jobs complete (either
successfully or not), they are removed from the graph and action taken. If
another job depends on the completed job having succeeded, then that dependent
job will be cancelled. Any newly-uncovered leaves now run. Some event triggers
may also be invoked on completion of a job, usually within the JavaScript layer.
For example, an object may specify an "on-failure" job to run if it fails to
start.

The result of a transaction is the result of its goal job.

### Process Manager (PROCMAN)

Carries out the low-level tasks of OS process management. Processes are observed
and on GNU/Linux and BSD platforms, this extends to tracking processes as they
create subprocesses and exit (Kernel Queues or CGroups can do this.)

Process launch may be hooked by Process Launch Extensions to alter the execution
environment prior to launching an actual subprocess.

### Service Virtual Machines Monitor (SVMMON)

Governs JavaScript virtual machines provided by the QuickJS library. Extensive
integration with the other core services is provided for. A small library of
functionality to integrate with the operating system directly is provided; the
interfaces are mostly similar to those of Node.JS. A WebWorkers-like API
provides for out-of-process modules to run.

### Reference Monitor (REFMON)

This verifies that requests originating outwith the InitKit scheduler service
hold a right to query a particular object. All connections to the InitKit over
the management interface are associated with a set of rights, e.g. to start or
stop a particular object, to query any object's status, or to shut down the
service manager. Any request made over the management interface must be
authorised by the Reference Monitor before the action can be carried out.

### Schedulable Object Graph (OBJGRAPH)

All objects in the system are maintained in an object graph. This graph
maintains a description of each object sufficient for the other core services to
operate with; this includes the state of the object, its interrelations (edges)
with other objects, and where the various data describing the object was sourced
from.

## JavaScript Services

Various higher-level services are written in JavaScript and run in the virtual
machines.

### InitKit Subsystem for Systemd Services (ISSS)

These components implement support for 

#### Systemd Manifest Loader

This component loads legacy systemd unit-files.

#### Delegated Restarter for Services

Implements the supervision semantics of systemd `.service` units.

Licencing
---------

This software is made available under the terms of the 4-Clause BSD Licence. See
[LICENCE.md](LICENCE.md). If you do not wish to use the software under those
terms, you may use it under the terms of the Affero GPL version 3.0 instead.