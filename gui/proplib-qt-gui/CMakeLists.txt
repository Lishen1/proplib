cmake_minimum_required(VERSION 2.8)
project(proplib-qt-gui)


find_package(Qt5 COMPONENTS Widgets )
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}") 
#

set ( MOC_HEADERS
 proplibqtgui.h
)

set ( UIS
 proplibqtgui.ui
)

set ( RESOURCES
 proplibqtgui.qrc
)

set(CMAKE_AUTOMOC TRUE)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UIS} )
add_library(proplib-qt-gui STATIC proplibqtgui.ui proplibqtgui.cpp ${MOC_HEADERS} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${UIS} ui_proplibqtgui.h)
target_include_directories(proplib-qt-gui PRIVATE ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Declarative_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/contrib ${CMAKE_SOURCE_DIR}/contrib/yaml-cpp/include ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(proplib-qt-gui PRIVATE ${PROP_TARGET} ${Qt5Widgets_LIBRARIES} ${Qt5Declarative_LIBRARIES} staticQtnProperty)
#file(COPY "${AUTOGEN_BUILD_DIR}/ui_proplibqtgui.h" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/Generated")
