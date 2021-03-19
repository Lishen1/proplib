add_executable( editor
    include/editor/EditorWindow.hpp
    EditorWindow.cpp
    main.cpp
)

target_include_directories( editor PRIVATE include)
target_link_libraries( editor PRIVATE ${PROPLIB_NAME} propimgui)

set_target_properties(editor PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROPLIB_OUTPUT_DIR}"
                                        RUNTIME_OUTPUT_DIRECTORY "${PROPLIB_OUTPUT_DIR}")