project ( proplib-qt-editor )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui Script REQUIRED)
add_executable ( qt-editor
  main.cpp
  yaml_highlighter.cpp
  proplibqteditor.cpp
  #proplibqteditor.ui
# proplib-qt-editor.ico
# proplib-qt-editor.rc
 proplibqteditor.qrc
)


add_subdirectory(include)
target_include_directories ( qt-editor PRIVATE include)
target_link_libraries( qt-editor PRIVATE propsdk qt-gui-module Qt5::Widgets Qt5::Gui Qt5::Script)

