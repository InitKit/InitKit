cmake_minimum_required (VERSION 2.8)
project (rpc)

function (GenRPCXdr path)
   GET_FILENAME_COMPONENT (file ${path} NAME_WE)
   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     COMMAND rpcgen -CNh ${path}
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     DEPENDS ${path})

   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     COMMAND rpcgen -CNc ${path}
        | sed 's/.h"/_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     DEPENDS ${path} ${file}_rpc.h)

   add_library (${file}xdr STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c)

   add_custom_target (${file}-rpc-xdr ALL DEPENDS 
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h)

endfunction (GenRPCXdr)

function (GenRPC path)
   GET_FILENAME_COMPONENT (file ${path} NAME_WE)

   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     COMMAND rpcgen -CNc ${path}
        | sed 's/.h"/_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
     COMMAND rpcgen -CNl ${path}
        | sed 's/.h"/_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
     COMMAND rpcgen -CNm ${path}
        | sed 's/.h"/_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
     DEPENDS ${path} ${file}_rpc.h)

   add_custom_command (
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     COMMAND rpcgen -CNh ${path}
        | sed 's/.h"/_rpc.h"/'
        > ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h
     DEPENDS ${path})

   add_custom_target (${file}-rpc ALL DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_rpc.h)

   add_library (${file}rpc SHARED 
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_xdr.c
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_clnt.c)

   add_library (${file}svc STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/${file}_svc.c)

   target_link_libraries (${file}rpc s16xdr)
   add_dependencies (${file}rpc s16-rpc-xdr)
   add_dependencies (${file}svc s16-rpc-xdr)
   target_link_libraries (${file}svc ${file}rpc)
endfunction (GenRPC)

GenRPCXdr (${PROJECT_SOURCE_DIR}/s16.x)
GenRPC (${PROJECT_SOURCE_DIR}/repositoryd.x)
GenRPC (${PROJECT_SOURCE_DIR}/restartd.x)
GenRPC (${PROJECT_SOURCE_DIR}/config-subscriber.x)