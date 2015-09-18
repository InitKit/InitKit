cmake_minimum_required (VERSION 2.8)
project (s16.repositoryd)

add_executable (s16.repositoryd interface.c notify.c object.c repositoryd.c)
target_link_libraries (s16.repositoryd s16 s16db s16rr repositorydsvc 
                       config-subscriberrpc)