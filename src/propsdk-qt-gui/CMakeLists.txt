
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
add_git_dependency(
  TARGET
    qt-gui-module
  TARGET_ACCESS
    PUBLIC
  BINARY_DIRECTORY
    ${PROPLIB_OUTPUT_DIR}
  NAME QtnProperty
  GITHUB_REPOSITORY ueberaccelerate/QtnProperty
  GIT_TAG master
  OPTIONS
  #  "YAML_CPP_BUILD_TESTS Off"
  #  "YAML_CPP_BUILD_CONTRIB Off"
  #  "YAML_CPP_BUILD_TOOLS Off"
  IMPORT_LIBS
    QtnProperty
)
add_git_dependency(
  TARGET
    qt-gui-module
  TARGET_ACCESS
    PUBLIC
  BINARY_DIRECTORY
    ${PROPLIB_OUTPUT_DIR}
  NAME cpp-httplib
  GITHUB_REPOSITORY yhirose/cpp-httplib
  GIT_TAG b845425 # 0.8.5
  #VERSION 0.8.5
  IMPORT_LIBS
    httplib
)

add_subdirectory(include)
target_link_libraries( qt-gui-module PRIVATE propsdk Qt5::Widgets  )
target_include_directories( qt-gui-module PUBLIC include )



