cmake_minimum_required(VERSION 3.14)
project(daemon-manager)
include(GNUInstallDirs)

set (CMAKE_CXX_STANDARD 11)

list (APPEND DAEMON_PUBLIC_HEADERS daemon_server.hpp
                                   daemon_client.hpp     
                                   daemon_server.ipp
                                   daemon_client.ipp
                                   common.hpp
                                   serializer.hpp)


find_package(ZeroMQ REQUIRED)
find_package(zmqpp REQUIRED)

add_library(daemon_manager INTERFACE)
set_target_properties(daemon_manager PROPERTIES PUBLIC_HEADER "${DAEMON_PUBLIC_HEADERS}")
target_link_libraries(daemon_manager INTERFACE zmqpp libzmq)

install(TARGETS daemon_manager
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/daemon_manager)

add_subdirectory(python)
