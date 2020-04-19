find_path(gmock_INCLUDE_DIR gmock/gmock.h)
find_library(gmock_LIBRARY NAMES gmock)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        gmock
        DEFAULT_MSG
        gmock_LIBRARY
        gmock_INCLUDE_DIR)

if(gmock_FOUND)
    set(gmock_LIBRARIES ${gmock_LIBRARY})
    set(gmock_INCLUDE_DIRS ${gmock_INCLUDE_DIR})

    if(NOT TARGET gmock)
        add_library(gmock UNKNOWN IMPORTED)
        set_target_properties(gmock PROPERTIES
                IMPORTED_LOCATION "${gmock_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${gmock_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(gmock_INCLUDE_DIR gmock_LIBRARY)

