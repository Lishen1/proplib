project(propimgui)

add_library( propimgui INTERFACE)
target_include_directories( propimgui INTERFACE include )
target_link_libraries( propimgui INTERFACE propsdk )

#set_target_properties( propimgui
#    PROPERTIES
#    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
#    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
#)
