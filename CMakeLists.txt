INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists)

cmake_minimum_required(VERSION 2.8.0)

set(WARN_FLAGS "-Wall -Wno-unused-function -Wno-switch")
# add flags used in the entire project
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -std=c11 -fPIC ${WARN_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -std=c++11 -fPIC ${WARN_FLAGS}")

CHECK_INCLUDE_FILES (threads.h C11_THREADS)

if(NOT C11_THREADS)
	message("Using internal implementation of C11 threads")
	include_directories("${PROJECT_SOURCE_DIR}/external/c11thrd")
endif()

CHECK_LIBRARY_EXISTS(stdthreads "mtx_lock" "" LIBSTDTHREADS)

if(UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
    set(LINUX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*|FreeBSD")
    set(FREEBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
    set(NETBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
    set(OPENBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*")
    set(DRAGONFLY TRUE)
  endif()
elseif(APPLE)
  set (OSX TRUE)
endif()

include_directories("${PROJECT_SOURCE_DIR}/external/ini/hdr")

add_subdirectory(external/ini)

include_directories("${PROJECT_SOURCE_DIR}/hdr")
include_directories("${PROJECT_BINARY_DIR}/rpc")

add_subdirectory(rpc)

add_subdirectory(lib/s16)
add_subdirectory(lib/s16db)
add_subdirectory(lib/s16rr)

add_subdirectory(app/restartd)
add_subdirectory(app/repositoryd)
add_subdirectory(app/svccfg)
add_subdirectory(app/svcorder)
