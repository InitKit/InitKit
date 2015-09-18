cmake_minimum_required (VERSION 2.8)
project (s16.restartd)

add_executable (s16.restartd configd.c confsub.c dirwatch.c interface.c
                restartd.c timer.c unit.c)
target_link_libraries (s16.restartd s16 s16db s16rr config-subscribersvc
                       restartdsvc)