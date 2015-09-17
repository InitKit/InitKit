cmake_minimum_required(VERSION 2.8)
project (rpc)

function (GenRPCHdr path)

   GET_FILENAME_COMPONENT(file ${path} NAME_WE)
   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     COMMAND rpcgen -CNh ${path}
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     DEPENDS ${path})

   add_custom_target(${file}-rpc-hdr ALL DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h)

endfunction (GenRPCHdr)

function (GenRPC path)

   GET_FILENAME_COMPONENT(file ${path} NAME_WE)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     COMMAND rpcgen -CNc ${path}
        | sed 's/${file}.h"/${file}_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
     COMMAND rpcgen -CNl ${path}
        | sed 's/${file}.h"/${file}_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
     COMMAND rpcgen -CNm ${path}
        | sed 's/${file}.h"/${file}_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     COMMAND rpcgen -CNh ${path}
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     DEPENDS ${path})

   add_custom_target(${file}-rpc ALL DEPENDS 
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h)

endfunction (GenRPC)

GenRPCHdr(${PROJECT_SOURCE_DIR}/s16.x)
GenRPC(${PROJECT_SOURCE_DIR}/repositoryd.x)
GenRPC(${PROJECT_SOURCE_DIR}/restartd.x)
GenRPC(${PROJECT_SOURCE_DIR}/config-subscriber.x)

set(repodrpc_SRCS
${CMAKE_CURRENT_BINARY_DIR}/repositoryd_xdr.c
)

add_library(repodrpc SHARED ${repodrpc_SRCS})