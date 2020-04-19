find_path(zmqpp_INCLUDE_DIR zmqpp/zmqpp.hpp)
find_library(zmqpp_LIBRARY NAMES zmqpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        zmqpp
        DEFAULT_MSG
        zmqpp_LIBRARY
        zmqpp_INCLUDE_DIR)

if(zmqpp_FOUND)
    set(zmqpp_LIBRARIES ${zmqpp_LIBRARY})
    set(zmqpp_INCLUDE_DIRS ${zmqpp_INCLUDE_DIR})

    if(NOT TARGET zmqpp)
        add_library(zmqpp UNKNOWN IMPORTED)
        set_target_properties(zmqpp PROPERTIES
                IMPORTED_LOCATION "${zmqpp_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${zmqpp_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(zmqpp_INCLUDE_DIR zmqpp_LIBRARY)

