cmake_minimum_required (VERSION 2.8)
project (svccfg)

add_executable (svccfg parse-unit.c svccfg.c)
target_link_libraries (svccfg s16 s16db ini)