

add_library( propimgui INTERFACE)
target_include_directories( propimgui INTERFACE include )

add_git_dependency(
  TARGET
    propimgui
  TARGET_ACCESS
    PUBLIC
  BINARY_DIRECTORY
    ${PROPLIB_OUTPUT_DIR}
  NAME              imgui
  GITHUB_REPOSITORY ueberaccelerate/imgui
  GIT_TAG           master
  OPTIONS           "IMGUI_BUILD_EXAMPLES OFF"
)

add_git_dependency(
  TARGET
    propimgui
  TARGET_ACCESS
    PUBLIC
  BINARY_DIRECTORY
    ${PROPLIB_OUTPUT_DIR}
  NAME              imgui-filebrowser
  GITHUB_REPOSITORY AirGuanZ/imgui-filebrowser
  GIT_TAG           master
  OPTIONS           "IMGUI_BUILD_EXAMPLES OFF"
)
target_include_directories( imgui PUBLIC ${PROPLIB_BINARY_DIR}/_deps/imgui-filebrowser-src)
target_link_libraries( propimgui INTERFACE ${PROPLIB_NAME} imgui yaml-cpp)