cmake_minimum_required (VERSION 2.8)
project (s16)

add_library (s16 SHARED list.c mem.c misc.c object.c)

if(LIBSTDTHREADS)
  message("Using Libstdthreads")
  target_link_libraries (s16 stdthreads)
else()
  target_link_libraries (s16 pthread)
endif()