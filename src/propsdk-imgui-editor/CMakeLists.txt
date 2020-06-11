project(editor)

add_executable( editor
    include/editor/EditorWindow.hpp
    EditorWindow.cpp
    main.cpp
)
target_include_directories( editor PRIVATE include )
target_link_libraries( editor PRIVATE propsdk )

set_target_properties( editor
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
