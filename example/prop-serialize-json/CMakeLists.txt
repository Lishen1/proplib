
add_target(
  TARGET
    prop-serialize-json
  SOURCES
    main.cpp
  PRIVATE_LINKS
    propsdk
  BINARY_DIRECTORY
    ${PROPLIB_OUTPUT_DIR}
  EXECUTABLE
)

