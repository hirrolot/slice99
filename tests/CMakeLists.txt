cmake_minimum_required(VERSION 3.0.2)
project(examples LANGUAGES C)

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
    GIT_REPOSITORY https://github.com/Hirrolot/assert-algebraic.git
    GIT_TAG master # TODO: change it.
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
