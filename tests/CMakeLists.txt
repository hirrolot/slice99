cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES C)

# Fix the warnings about `DOWNLOAD_EXTRACT_TIMESTAMP` in newer CMake versions.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

add_subdirectory(.. build)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Weverything -fsanitize=address)
  add_link_options(-fsanitize=address)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -pedantic -fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

add_executable(test test.c)

include(FetchContent)

FetchContent_Declare(
    assert-algebraic
    URL https://github.com/Hirrolot/assert-algebraic/archive/refs/tags/v0.2.0.tar.gz
)

FetchContent_MakeAvailable(assert-algebraic)

get_property(
  TESTS
  DIRECTORY .
  PROPERTY BUILDSYSTEM_TARGETS)

foreach(TARGET ${TESTS})
  target_link_libraries(${TARGET} slice99 assert-algebraic)
  set_target_properties(${TARGET} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED
                                                         ON)
endforeach()
