# System XVI

![System XVI Logo](doc/logotype_variant.png)

### Overview
System XVI is a modern take on service management.
It aims to incorporate useful functionality while maintaining a modular 
design in the UNIX tradition.

This project is still under active development. It is not yet possible to
boot with System XVI; indeed, the init subdirectory is a placeholder.
All effort is currently focused on bringing the various components up to
a level where they are able to act as a service supervisor tool capable of 
running simple, forking, and oneshot systemd units.

System XVI is designed for modularity. Please find a flowchart depicting
the key components' actions and interactions [here](doc/s16.png).

### Development

##### Building
System XVI uses kBuild (as used in VirtualBox, etc).
There are minimal dependencies: you need procps and libkqueue in their 
development formats. 
Then just install your distro's package for kBuild (probably called 
kbuild) and run kmk.

##### Licence
System XVI is free software. You may use it and its source code under the 
terms of the GPL version 3.0 or (at your option) any later version.
Please find a copy of the GPL version 3.0 included in doc/CPYRIGHT
