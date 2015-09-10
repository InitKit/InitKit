cmake_minimum_required(VERSION 2.8)
project(s16)
file(GLOB_RECURSE SRC_FILES "*.c")
add_library(s16 STATIC ${SRC_FILES})

if(NOT C11_THREADS)
    message("Linking libs16 with pthread")
    target_link_libraries(s16 pthread)
endif()

add_custom_command(
OUTPUT /$(CMAKE_BINARY_DIR)/repositoryd_rpc.h
COMMAND rpcgen -C -N -h -o $(CMAKE_BINARY_DIR)/repositoryd_rpc.h $(CMAKE_SOURCE_DIR)/repositoryd/repositoryd.x
)

add_custom_target(do_repositorydh DEPENDS /$(CMAKE_BINARY_DIR)/repositoryd_rpc.h)
add_dependencies(s16 do_repositorydh)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
include_directories(${PROJECT_SOURCE_DIR}/hdr)
install(TARGETS s16 ARCHIVE DESTINATION lib)
