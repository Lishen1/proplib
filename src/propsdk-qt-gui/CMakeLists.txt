project (qt-gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

add_library ( qt-gui-module 
#  main.cpp
  proplibqtgui.cpp
  proplibqtgui.qrc
  proplibqtgui.ui
)

add_subdirectory(include)
target_link_libraries( qt-gui-module PRIVATE propsdk Qt5::Widgets)
target_include_directories( qt-gui-module PUBLIC include )

