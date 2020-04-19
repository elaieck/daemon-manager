
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS daemon.proto)
protobuf_generate_python(PROTO_PYS daemon.proto)
add_custom_target(proto_example ALL DEPENDS ${PROTO_PYS})

add_executable(daemon_server_example "simple_server.cpp" ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(daemon_server_example daemon_manager ${PROTOBUF_LIBRARIES})

add_executable(daemon_client_example "simple_request_client.cpp" ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(daemon_client_example daemon_manager ${PROTOBUF_LIBRARIES})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon_pb2.py
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python2.7/site-packages)
