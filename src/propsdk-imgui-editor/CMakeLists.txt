project(editor)


add_executable( editor
    include/editor/EditorWindow.hpp
    
    
    EditorWindow.cpp
    
    main.cpp
)
target_include_directories( editor PRIVATE include )
target_link_libraries( editor PRIVATE propsdk )
