cmake_minimum_required(VERSION 3.8)
project(pydaemon VERSION 0.3.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)

find_package(Python2 COMPONENTS Interpreter Development)

add_library(pydaemon SHARED pydaemon.cpp)

set_target_properties(pydaemon PROPERTIES PREFIX "")
set_target_properties(pydaemon PROPERTIES OUTPUT_NAME "pydaemon")

target_compile_options(pydaemon PRIVATE -fvisibility=hidden)

target_include_directories(
    pydaemon PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_include_directories(pydaemon PRIVATE ${Python2_INCLUDE_DIRS})

target_link_libraries(pydaemon PUBLIC daemon_manager ${Python2_LIBRARIES})

# We explicitly use "python 2.7" since the version we use of pybind11, binds
# us to a specific python version
install(TARGETS pydaemon
        EXPORT pydaemonConfig
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python2.7/site-packages)
