set (PROP_CONTRIB_HEADER 
  ${CMAKE_CURRENT_SOURCE_DIR}/Easyloggingpp/easylogging++.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Easyloggingpp/easylogging++.cc )
 


add_subdirectory(yaml-cpp)
add_subdirectory(QtnProperty)

target_include_directories(${PROP_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Easyloggingpp/)
target_include_directories(${PROP_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include)

target_sources(${PROP_TARGET} PRIVATE ${PROP_CONTRIB_HEADER})