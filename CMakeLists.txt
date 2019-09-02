cmake_minimum_required(VERSION 3.0)
project(proplib)

option(BUILD_TOOLS "Build tools" TRUE)
option(BUILD_TEST "Build Test Folder" TRUE)
option(BUILD_GUI "Build GUI frontend" TRUE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(PROP_VERSION 0.0.0)
set(PROP_TARGET proplib_${PROP_VERSION})
message(STATUS "${CMAKE_MODULE_PATH}")
message(STATUS "Version: ${PROP_VERSION}")

add_library(${PROP_TARGET} STATIC "")
target_include_directories(${PROP_TARGET} PUBLIC include)
target_link_libraries(${PROP_TARGET} PRIVATE yaml-cpp)


add_subdirectory(include)
add_subdirectory(contrib)
if(BUILD_TEST)
  add_subdirectory(tests)
#  add_executable(sanity_checks src/sanity_checks.cpp)
#  target_link_libraries(sanity_checks ${LIBS})
#  add_executable(test_num_exceptions samples/test_num_exceptions.cpp)
#  target_link_libraries(test_num_exceptions ${LIBS} ${CHAISCRIPT_LIBS})
#  add_executable(memory_leak_test samples/memory_leak_test.cpp)
#  target_link_libraries(memory_leak_test ${LIBS} ${CHAISCRIPT_LIBS})
#  add_executable(inheritance samples/inheritance.cpp)
#  target_link_libraries(inheritance ${LIBS} ${CHAISCRIPT_LIBS})
#  add_executable(factory samples/factory.cpp)
#  target_link_libraries(factory ${LIBS} ${CHAISCRIPT_LIBS})
#  add_executable(fun_call_performance samples/fun_call_performance.cpp)
#  target_link_libraries(fun_call_performance ${LIBS} ${CHAISCRIPT_LIBS})
endif()
if(BUILD_TOOLS)
  add_subdirectory(tools)
  #add_library(test_module MODULE src/test_module.cpp)
  #target_link_libraries(test_module ${LIBS})
  #
  #add_library(stl_extra MODULE src/stl_extra.cpp)
  #target_link_libraries(stl_extra ${LIBS})
  #
  #set(MODULES stl_extra)
endif()

if(BUILD_GUI)
  add_subdirectory(gui)
  #add_library(test_module MODULE src/test_module.cpp)
  #target_link_libraries(test_module ${LIBS})
  #
  #add_library(stl_extra MODULE src/stl_extra.cpp)
  #target_link_libraries(stl_extra ${LIBS})
  #
  #set(MODULES stl_extra)
endif()

#cmake_minimum_required(VERSION 2.8)
#
#if(NOT ${CMAKE_VERSION} VERSION_LESS "3.1")
#  cmake_policy(SET CMP0054 NEW)
#endif()
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#project(proplib)
#
#option(MULTITHREAD_SUPPORT_ENABLED "Multithreaded Support Enabled" FALSE)
#
#
#option(BUILD_TOOLS "Build tools" TRUE)
#option(BUILD_TEST "Build Test Folder" TRUE)
#
#if(CMAKE_COMPILER_IS_GNUCC)
#  option(ENABLE_COVERAGE "Enable Coverage Reporting in GCC" FALSE)
#
#  if(ENABLE_COVERAGE)
#    add_definitions(--coverage -O0)
#    set(LINKER_FLAGS "${LINKER_FLAGS} --coverage")
#  endif()
#endif()
#
#if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#  option(ENABLE_THREAD_SANITIZER "Enable thread sanitizer testing in gcc/clang" FALSE)
#  if(ENABLE_THREAD_SANITIZER)
#    add_definitions(-fsanitize=thread -g)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=thread")
#  endif()
#
#  option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer testing in gcc/clang" FALSE)
#  if(ENABLE_ADDRESS_SANITIZER)
#    add_definitions(-fsanitize=address -g)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
#
#    option(BUILD_LIBFUZZ_TESTER "Build libfuzzer tool" FALSE)
#  endif()
#
#  option(ENABLE_MEMORY_SANITIZER "Enable memory sanitizer testing in gcc/clang" FALSE)
#  if(ENABLE_MEMORY_SANITIZER)
#    add_definitions(-fsanitize=memory -fsanitize-memory-track-origins -g)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins ")
#  endif()
#
#  option(ENABLE_UNDEFINED_SANITIZER "Enable undefined behavior sanitizer testing in gcc/clang" FALSE)
#  if(ENABLE_UNDEFINED_SANITIZER)
#    add_definitions(-fsanitize=undefined -g)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=undefined")
#  endif()
#
#  option(ENABLE_LTO "Enable Link Time Optimization" FALSE)
#
#  if (ENABLE_LTO)
#    add_definitions(-flto)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -flto")
#  endif()
#
#  option(GPROF_OUTPUT "Generate profile data" FALSE)
#  if (GPROF_OUTPUT)
#    add_definitions(-pg)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -pg")
#  endif()
#
#
#  option(PROFILE_GENERATE "Generate profile data" FALSE)
#  if (PROFILE_GENERATE)
#    add_definitions(-fprofile-generate)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fprofile-generate")
#  endif()
#
#  option(PROFILE_USE "Use profile data" FALSE)
#  if (PROFILE_USE)
#    add_definitions(-fprofile-use)
#    set(LINKER_FLAGS "${LINKER_FLAGS} -fprofile-use")
#  endif()
#
#
#endif()
#
#list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}")
#list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.svn")
#list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.git")
#list(APPEND CPACK_SOURCE_IGNORE_FILES ".swp")
#list(APPEND CPACK_SOURCE_IGNORE_FILES ".*~")
#
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/readme.md")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/description.txt")
#
#set(CPACK_PACKAGE_VERSION_MAJOR 0)
#set(CPACK_PACKAGE_VERSION_MINOR 0)
#set(CPACK_PACKAGE_VERSION_PATCH 1)
#
##set(CPACK_PACKAGE_EXECUTABLES "chai;ChaiScript Eval")
#set(CPACK_PACKAGE_VENDOR "PavlishkaIndst.com")
#set(CPACK_PACKAGE_CONTACT "contact@PavlishkaIndst.com")
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An serialize and tool for C++")
#
#set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
#set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
#
#set(CPACK_RPM_PACKAGE_LICENSE "BSD")
#set(CPACK_RPM_PACKAGE_GROUP "Programming")
#
#set(PROP_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
#
#configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
#
##include(CTest)
##include(CPack)
##include(cmake/Catch.cmake)
#
##enable_testing()
#
#if(CMAKE_COMPILER_IS_GNUCC)
#  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
#
#  set(CPP17_FLAG "-std=c++1z")
#endif()
#
#if(MSVC)
#  add_definitions(/std:c++latest /W4 /w14545 /w34242 /w34254 /w34287 /w44263 /w44265 /w44296 /w44311 /w44826 /we4289 /w14546 /w14547 /w14549 /w14555 /w14619 /w14905 /w14906 /w14928)
#
#  #add_definitions(/std:c++17)
#
#
#  if (MSVC_VERSION STREQUAL "1800")
#    # VS2013 doesn't have magic statics
#    add_definitions(/w44640)
#  else()
#    # enum warnings are too noisy on MSVC2013
#    add_definitions(/w34062)
#  endif()
#
#  add_definitions(/bigobj /permissive-)
#  # Note on MSVC compiler flags.
#  # The code base selective disables warnings as necessary when the compiler is complaining too much
#  # about something that is perfectly valid, or there is simply no technical way around it
#  # This particular warning, C4503 is in regards to the decorated names that MSVC generates internally.
#  # The error did not come up until the move to C++11, but the compiler doesn't give enough information
#  # to determine where the error is coming from, and the internet provides no real information for
#  # how to workaround or fix the error. So I'm disabling it globally.
#  add_definitions(/wd4503)
#else()
#  add_definitions(-Wall -Wextra -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual -Wunused -Woverloaded-virtual -Wno-noexcept-type -Wpedantic ${CPP17_FLAG})
#
#  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    add_definitions(-Weverything -Wno-c++98-compat-pedantic  -Wno-c++98-compat -Wno-documentation -Wno-switch-enum -Wno-weak-vtables -Wno-missing-prototypes -Wno-padded -Wno-missing-noreturn -Wno-exit-time-destructors -Wno-documentation-unknown-command -Wno-unused-template -Wno-undef )
#  else()
#    add_definitions(-Wnoexcept)
#  endif()
#
#  if(APPLE)
#    add_definitions(-Wno-sign-compare)
#  endif()
#endif()
#
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#  option(USE_LIBCXX "Use clang's libcxx" TRUE)
#
#  if(USE_LIBCXX)
#    add_definitions(-stdlib=libc++)
#    set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP17_FLAG} -stdlib=libc++")
#  else()
#    set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP17_FLAG}")
#  endif()
#elseif(CMAKE_COMPILER_IS_GNUCC)
#  set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP17_FLAG}")
#endif()
#
## limitations in MinGW require us to make an optimized build
## for the sake of object sizes or something
#if(MINGW OR CYGWIN)
#  add_definitions(-O3)
#endif()
#
#
#
##set(Chai_INCLUDES include/chaiscript/chaiscript.hpp include/chaiscript/chaiscript_threading.hpp include/chaiscript/dispatchkit/bad_boxed_cast.hpp include/chaiscript/dispatchkit/bind_first.hpp include/chaiscript/dispatchkit/bootstrap.hpp include/chaiscript/dispatchkit/bootstrap_stl.hpp include/chaiscript/dispatchkit/boxed_cast.hpp include/chaiscript/dispatchkit/boxed_cast_helper.hpp include/chaiscript/dispatchkit/boxed_number.hpp include/chaiscript/dispatchkit/boxed_value.hpp include/chaiscript/dispatchkit/dispatchkit.hpp include/chaiscript/dispatchkit/type_conversions.hpp include/chaiscript/dispatchkit/dynamic_object.hpp include/chaiscript/dispatchkit/exception_specification.hpp include/chaiscript/dispatchkit/function_call.hpp include/chaiscript/dispatchkit/function_call_detail.hpp include/chaiscript/dispatchkit/handle_return.hpp include/chaiscript/dispatchkit/operators.hpp include/chaiscript/dispatchkit/proxy_constructors.hpp include/chaiscript/dispatchkit/proxy_functions.hpp include/chaiscript/dispatchkit/proxy_functions_detail.hpp include/chaiscript/dispatchkit/register_function.hpp include/chaiscript/dispatchkit/type_info.hpp include/chaiscript/language/chaiscript_algebraic.hpp include/chaiscript/language/chaiscript_common.hpp include/chaiscript/language/chaiscript_engine.hpp include/chaiscript/language/chaiscript_eval.hpp include/chaiscript/language/chaiscript_parser.hpp include/chaiscript/language/chaiscript_prelude.hpp include/chaiscript/language/chaiscript_prelude_docs.hpp include/chaiscript/utility/utility.hpp include/chaiscript/utility/json.hpp include/chaiscript/utility/json_wrap.hpp)
##set_source_files_properties(${Chai_INCLUDES} PROPERTIES HEADER_FILE_ONLY TRUE)
#
#
#if(CMAKE_HOST_UNIX)
#
#  if(MULTITHREAD_SUPPORT_ENABLED)
#    if(CMAKE_COMPILER_IS_GNUCC)
#      execute_process(COMMAND ${CMAKE_C_COMPILER} --version OUTPUT_VARIABLE GCC_FULL_VERSION)
#      if(GCC_FULL_VERSION MATCHES "4.8.1.*ubuntu")
#        set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--no-as-needed -pthread")
#      else()
#        set(LINKER_FLAGS "${LINKER_FLAGS} -pthread")
#      endif()
#    else()
#      set(LINKER_FLAGS "${LINKER_FLAGS} -pthread")
#    endif()
#
#    add_definitions(-pthread)
#  endif()
#
#endif()
#
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")
#
#add_library(proplib_${PROP_VERSION} STATIC "")
#target_link_libraries(proplib_${PROP_VERSION} yaml-cpp)
##target_include_directories (proplib_${PROP_VERSION} PRIVATE include contrib/Easyloggingpp contrib/yaml-cpp/include)
#include_directories(include)
#include_directories(contrib/Easyloggingpp)
#include_directories(contrib/yaml-cpp/include)
#
#
#add_subdirectory(include)
#add_subdirectory(contrib)
#
#
#message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "" FILES ${PROP_SERIALIZE_HEADER})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/contrib PREFIX "" FILES ${PROP_CONTRIB_HEADER})
##add_library(parser STATIC static_libs/chaiscript_parser.cpp)
#
##add_library(chaiscript_stdlib-${CHAI_VERSION} MODULE src/chaiscript_stdlib_module.cpp)
##target_link_libraries(chaiscript_stdlib-${CHAI_VERSION} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
#
##set(CHAISCRIPT_LIBS stdlib parser)
#
##add_executable(chai src/main.cpp ${Chai_INCLUDES})
##target_link_libraries(chai ${LIBS} ${CHAISCRIPT_LIBS})
#
#if(BUILD_TEST)
#  add_subdirectory(tests)
##  add_executable(sanity_checks src/sanity_checks.cpp)
##  target_link_libraries(sanity_checks ${LIBS})
##  add_executable(test_num_exceptions samples/test_num_exceptions.cpp)
##  target_link_libraries(test_num_exceptions ${LIBS} ${CHAISCRIPT_LIBS})
##  add_executable(memory_leak_test samples/memory_leak_test.cpp)
##  target_link_libraries(memory_leak_test ${LIBS} ${CHAISCRIPT_LIBS})
##  add_executable(inheritance samples/inheritance.cpp)
##  target_link_libraries(inheritance ${LIBS} ${CHAISCRIPT_LIBS})
##  add_executable(factory samples/factory.cpp)
##  target_link_libraries(factory ${LIBS} ${CHAISCRIPT_LIBS})
##  add_executable(fun_call_performance samples/fun_call_performance.cpp)
##  target_link_libraries(fun_call_performance ${LIBS} ${CHAISCRIPT_LIBS})
#endif()
#
#
#if(BUILD_TOOLS)
#  add_subdirectory(tools)
#  #add_library(test_module MODULE src/test_module.cpp)
#  #target_link_libraries(test_module ${LIBS})
#  #
#  #add_library(stl_extra MODULE src/stl_extra.cpp)
#  #target_link_libraries(stl_extra ${LIBS})
#  #
#  #set(MODULES stl_extra)
#endif()
#
#
##file(GLOB UNIT_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/unittests/ ${CMAKE_CURRENT_SOURCE_DIR}/unittests/*.chai ${CMAKE_CURRENT_SOURCE_DIR}/unittests/3.x/*.chai)
##list(SORT UNIT_TESTS)
##
##file(GLOB PERFORMANCE_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/performance_tests/ ${CMAKE_CURRENT_SOURCE_DIR}/performance_tests/*.chai)
##list(SORT PERFORMANCE_TESTS)
#
#
#
##install(TARGETS chai chaiscript_stdlib-${CHAI_VERSION} ${MODULES} RUNTIME DESTINATION bin LIBRARY DESTINATION lib/chaiscript)
##
##install(DIRECTORY include/chaiscript DESTINATION include
##  PATTERN "*.hpp"
##  PATTERN "*/.svn*" EXCLUDE
##  PATTERN "*/.git*" EXCLUDE
##  PATTERN "*~" EXCLUDE)
##install(DIRECTORY unittests DESTINATION share/chaiscript
##  PATTERN "*.chai"
##  PATTERN "*.inc"
##  PATTERN "*/.svn*" EXCLUDE
##  PATTERN "*/.git*" EXCLUDE
##  PATTERN "*~" EXCLUDE)
##install(DIRECTORY samples DESTINATION share/chaiscript
##  PATTERN "*.chai"
##  PATTERN "*/.svn*" EXCLUDE
##  PATTERN "*/.git*" EXCLUDE
##  PATTERN "*~" EXCLUDE)
##
##configure_file(contrib/pkgconfig/chaiscript.pc.in lib/pkgconfig/chaiscript.pc @ONLY)
##install(FILES "${chaiscript_BINARY_DIR}/lib/pkgconfig/chaiscript.pc"
##        DESTINATION lib/pkgconfig)
##
#
