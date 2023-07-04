
add_subdirectory( prop-serialize-yaml )
if(BUILD_WITH_JSON_SERDESER)
	add_subdirectory( prop-serialize-json )
endif()

