INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists)

cmake_minimum_required(VERSION 2.8.0)

# add flags used in the entire project
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -std=c11 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -std=c++11 -Wall")

CHECK_INCLUDE_FILES (threads.h C11_THREADS)

if(NOT C11_THREADS)
	message("Using internal implementation of C11 threads")
	include_directories("${PROJECT_SOURCE_DIR}/external/c11thrd")
endif()

CHECK_LIBRARY_EXISTS(stdthreads "mtx_lock" "" LIBSTDTHREADS)

include_directories("${PROJECT_SOURCE_DIR}/hdr")
include_directories("${PROJECT_BINARY_DIR}/rpc")

add_subdirectory(rpc)