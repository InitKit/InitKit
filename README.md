# System XVI

![System XVI Logo](doc/logotype.png)

### Overview

*A note: please be aware that System XVI is not yet a usable system.*

System XVI is a modern take on service management.  It aims to incorporate
useful functionality while maintaining a modular design in the UNIX tradition.

We have an IRC channel; find it at irc://irc.freenode.net/systemxvi

*This project is still under heavy development*. It is *not yet possible* to boot
with System XVI; indeed, the init subdirectory is a placeholder.  All effort is
currently focused on bringing the various components up to a level where they
are able to act as a service supervisor tool capable of running simple,
forking, and oneshot systemd units.

System XVI is designed for modularity. Please find a flowchart depicting the
key components' actions and interactions [here](doc/s16.png).

### Development

##### Building
System XVI uses kBuild (as used in VirtualBox, etc).  There are a minimal
number of dependencies: you need procps and libkqueue in their development
formats. Then just install your distro's package for kBuild (probably called
kbuild) and run kmk.

##### Licence
System XVI is free software. You may use it and its source code under the terms
of the GPL version 3.0 or (at your option) any later version.  Please find a
copy of the GPL version 3.0 included in doc/CPYRIGHT


### FAQ
A few frequently asked questions about S16:

> On what platforms does S16 run?

 * FreeBSD
 * GNU+Linux
 * muslibc+BusyBox+Linux

It is likely that this will run on all of the other BSD systems as well.

> Why is it bigger than sinit?

Because System XVI's scope is greater than sinit. sinit simply initialises a
system sufficiently to call into /etc/rc or similar.  In contrast, S16 is a
service management system first and foremost, and an extensible one at that.

> Why do you like to use macros for simple tasks?

Because I don't like to type.

> Why do you use so many typedefs?

As above.

> What are your problems with systemd?

In a word: architectural. systemd is tightly integrated in a way that makes
pluggable replacements difficult. It also tends toward a monolithic design.
Whilst systemd's PID 1 may not be as big as some claim - systemd is, after all,
split into 69 binaries - it still includes complex logic for parsing
configuration files, handling D-Bus calls, a transactional dependency
resolution system, et al.

> What are System XVI's main design ideals?

 * Interface-orientation: the system should be designed to fit a clean and
   stable interface.
 * Seperation of concerns: individual components should not do much alone, but
   work in concert to create a grand system.
 * Pluggability: components should be easily replaceable.
 * Self-healing: components that crash should be able to restart without
   forgetting system state or otherwise causing breakage.

### Testimonials
System XVI has received unexpected publicity. Several commentators have written
these glowing endorsements of System XVI:

> This is probably the best example I've seen on how NOT to program in C.
> Unneeded typedefs, random macros for simple logic. Do not use.

 - /u/nwmcsween, reddit.

> It wont work, trust me.

 - /u/Risar, reddit, on the future functionality of System XVI.

> There's a lot of good reasons why this project will never be an alternative
> to systemd (or even a good piece of software in it's own right), and they're
> pretty much all shown during a discussion with the author on Rizon. [link to
> the main S16 developer drunkenly arguing with someone about code style.]

- /u/TheReverend403, reddit.
