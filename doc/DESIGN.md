# Design
A summary of the design of SystemXVI.

### libs16
libs16 provides a C interface to SystemXVI.

### repositoryd
s16.repositoryd is the configuration repository server.
It hosts the SystemXVI Configuration Repository, which stores services and their configuration in a form ready for the consumption of other components in the form of C structures, rather than as some sort of configuration file that needs parsing. Configuration parsing is a potentially dangerous task which is best implemented outwith core daemons; as a bonus, this makes the implementation of unit-file converters for systemd, or for any other format, much simpler.

### libs16db
libs16db provides low-level access to the SystemXVI Configuration Repository.
It provides a simple C API that allows a configuration repository to be queried, including the insertion and deletion of entries.
This library is of limited use to applications outwith SystemXVI itself; higher-level APIs are provided by other components of the stack.
