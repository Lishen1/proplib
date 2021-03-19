project( prop-serialize )

add_executable ( prop-serialize main.cpp )
target_link_libraries ( prop-serialize PRIVATE propsdk )

set_target_properties( prop-serialize
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
