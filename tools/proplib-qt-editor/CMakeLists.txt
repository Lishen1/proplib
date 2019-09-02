cmake_minimum_required(VERSION 2.8)
project(proplib-qt-editor)


find_package(Qt5 COMPONENTS Widgets )
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}") 
#

set ( MOC_HEADERS
proplibqteditor.h
)

set ( UIS
proplibqteditor.ui
)

set ( RESOURCES
proplibqteditor.qrc
)

set(CMAKE_AUTOMOC TRUE)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UIS} )
add_executable(proplib-qt-editor main.cpp proplibqteditor.ui  proplibqteditor.cpp yaml_highlighter.cpp ${MOC_HEADERS} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${UIS} ui_proplibqteditor.h)
target_include_directories(proplib-qt-editor PRIVATE ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Declarative_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/contrib ${CMAKE_SOURCE_DIR}/gui/proplib-qt-gui)
target_link_libraries(proplib-qt-editor PRIVATE ${PROP_TARGET} ${Qt5Widgets_LIBRARIES} ${Qt5Declarative_LIBRARIES} staticQtnProperty proplib-qt-gui)
#file(COPY "${AUTOGEN_BUILD_DIR}/ui_proplibqtgui.h" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/Generated")
