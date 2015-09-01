# Design
A summary of the design of SystemXVI.

### libs16
libs16 provides a C interface to SystemXVI.

### repositoryd
s16.repositoryd is the repository server.
It hosts the SystemXVI Configuration Repository, which stores services and their configuration. Access to the repository is available via libs16; this provides a C API allowing retrieval and insertion of data into the repository.
