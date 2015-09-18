cmake_minimum_required (VERSION 2.8)
project (s16db)

add_library (s16db SHARED rpc.c translate.c)
target_link_libraries (s16db repositorydrpc)