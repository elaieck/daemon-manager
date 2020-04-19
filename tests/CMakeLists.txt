cmake_minimum_required(VERSION 3.14)
project(daemon-manager)

find_package(GTest REQUIRED)
find_package(gmock REQUIRED)
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../examples/daemon.proto)
protobuf_generate_python(PROTO_PYS ../examples/daemon.proto)
add_custom_target(proto_test ALL DEPENDS ${PROTO_PYS})

enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(daemon_server_test "daemon_server_test.cpp" ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(daemon_server_test daemon_manager ${PROTOBUF_LIBRARIES} 
                      ${GTEST_BOTH_LIBRARIES} gmock ${CMAKE_THREAD_LIBS_INIT})
add_test(daemon_test daemon_server_test)

add_executable(daemon_client_test "daemon_request_client_test.cpp" ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(daemon_client_test daemon_manager ${PROTOBUF_LIBRARIES}
                      ${GTEST_BOTH_LIBRARIES} gmock ${CMAKE_THREAD_LIBS_INIT})
add_test(daemon_test daemon_client_test)

add_executable(daemon_subscriber_test "daemon_subscriber_test.cpp" ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(daemon_subscriber_test daemon_manager ${PROTOBUF_LIBRARIES}
                      ${GTEST_BOTH_LIBRARIES} gmock ${CMAKE_THREAD_LIBS_INIT})
add_test(daemon_test daemon_subscriber_test)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/daemon_pb2.py
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python2.7/site-packages)
