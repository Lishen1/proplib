
if (EDITOR_ENABLED )
  if ( QT_EDITOR_ENABLED )
    add_subdirectory( propsdk-qt-gui )
    add_subdirectory( propsdk-qt-editor )
  else()
    add_subdirectory( propsdk-imgui-gui )
    add_subdirectory( propsdk-imgui-editor )
  endif()
endif()

