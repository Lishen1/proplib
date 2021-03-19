
find_package(yaml-cpp CONFIG REQUIRED)

add_library(${PROPLIB_NAME} INTERFACE)

target_compile_features(
  ${PROPLIB_NAME} 
  INTERFACE 
    cxx_std_17 )
target_include_directories(
  ${PROPLIB_NAME} 
  INTERFACE 
    ${PROPLIB_SOURCE_DIR}/include)


if ( BUILD_PROPLIB_WITH_EDITOR )
  if ( BUILD_PROPLIB_WITH_QT_EDITOR )
    log(TEXT "Selected editor - qt" )
    add_subdirectory( propsdk-qt-gui )
    add_subdirectory( propsdk-qt-editor )
  else()
    log(TEXT "Selected editor - imgui" )
    add_subdirectory( propsdk-imgui-gui )
    add_subdirectory( propsdk-imgui-editor )
  endif()
endif()

