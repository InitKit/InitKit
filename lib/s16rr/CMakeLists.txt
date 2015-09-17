cmake_minimum_required (VERSION 2.8)
project (s16rr)

if (FREEBSD OR NETBSD OR OPENBSD OR DRAGONFLY)
  set (PT_DRIVER "kevent")
elseif (LINUX)
  set (PT_DRIVER "netlink")
else ()
  set (PT_DRIVER "posix")
endif()

add_library (s16rr SHARED process.c process-tracker/pt-driver-${PT_DRIVER}.c)
