include ( cmake/CPM.cmake )

CPMAddPackage(
  NAME easyloggingpp
  GITHUB_REPOSITORY amrayn/easyloggingpp
  VERSION 9.96.7
  OPTIONS "build_static_lib ON"
)

CPMAddPackage(
  NAME yaml-cpp
  GITHUB_REPOSITORY jbeder/yaml-cpp
  # 0.6.2 uses deprecated CMake syntax
  VERSION 0.6.3
  # 0.6.3 is not released yet, so use a recent commit
  GIT_TAG 012269756149ae99745b6dafefd415843d7420bb 
  OPTIONS
    "YAML_CPP_BUILD_TESTS Off"
    "YAML_CPP_BUILD_CONTRIB Off"
    "YAML_CPP_BUILD_TOOLS Off"
)

if ( TESTS_ENABLED )
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.10.x
    OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt"
  )
endif()

if ( QT_EDITOR_ENABLED )
  CPMAddPackage(
    NAME QtnProperty
    GITHUB_REPOSITORY ueberaccelerate/QtnProperty
    GIT_TAG master
    OPTIONS
    #  "YAML_CPP_BUILD_TESTS Off"
    #  "YAML_CPP_BUILD_CONTRIB Off"
    #  "YAML_CPP_BUILD_TOOLS Off"
  )
  CPMAddPackage(
    NAME cpp-httplib
    GITHUB_REPOSITORY yhirose/cpp-httplib
    VERSION 0.6.5
  )

  set ( TARGET_EDITOR QtnProperty httplib)
else()

  CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ueberaccelerate/imgui
    GIT_TAG master
    OPTIONS 
    "IMGUI_BUILD_EXAMPLES OFF"
  )
  CPMAddPackage(
    NAME imgui-filebrowser
    GITHUB_REPOSITORY AirGuanZ/imgui-filebrowser
    GIT_TAG master
    OPTIONS 
    "IMGUI_BUILD_EXAMPLES OFF"
  )
  target_include_directories( imgui PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/_deps/imgui-filebrowser-src)
  set ( TARGET_EDITOR imgui )
endif()

function (set_dependency target_name target_type ) 

# easyloggingpp headers bug manually add
target_include_directories( ${target_name} ${target_type} ${CMAKE_CURRENT_BINARY_DIR}/_deps/easyloggingpp-src/src )

target_link_libraries( ${target_name} ${target_type} easyloggingpp yaml-cpp ${TARGET_EDITOR} )

endfunction()
