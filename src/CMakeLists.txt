
find_package(yaml-cpp CONFIG REQUIRED)

if(BUILD_WITH_JSON_SERDESER)
  find_package(nlohmann_json CONFIG REQUIRED)
  set(JSON_LIBS nlohmann_json nlohmann_json::nlohmann_json)

endif()

add_library(${PROPLIB_NAME} INTERFACE)

if(PROPLIB_SUPER_DIR)
  target_sources(${PROPLIB_NAME}
    INTERFACE
      ${PROPLIB_SOURCE_DIR}/include/serialize/config.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/proplib.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/serializable.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/serialize.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/tools.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/types.h
      ${PROPLIB_SOURCE_DIR}/include/serialize/yaml_serialize.h
  )
endif()
if(BUILD_WITH_JSON_SERDESER)
	target_compile_definitions(
	  ${PROPLIB_NAME}
	  INTERFACE
	    BUILD_JSON_SERDESER
	)
endif()
target_compile_features(
  ${PROPLIB_NAME} 
  INTERFACE 
    cxx_std_17 )
target_include_directories(
  ${PROPLIB_NAME} 
  INTERFACE 
    ${PROPLIB_SOURCE_DIR}/include)
target_link_libraries(
  ${PROPLIB_NAME} 
  INTERFACE 
    yaml-cpp
    ${JSON_LIBS}
    )

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

