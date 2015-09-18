cmake_minimum_required (VERSION 2.8)
project (s16.svcorder)

add_executable (s16.svcorder confsub.c svcorder.c)
target_link_libraries (s16.svcorder s16 s16db config-subscribersvc)