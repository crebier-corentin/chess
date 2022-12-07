cmake_minimum_required(VERSION 3.20)
project(
  cocolib
  VERSION 1
  LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_CLANG_TIDY
  "clang-tidy;-checks=clang-analyzer-*,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

if(MSVC)
  add_compile_options(/W4)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-external:I") # fix issue with clang-tidy not

# recognizing -external:I with
# a space

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# add_compile_options(/fsanitize=address)
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF) # incompatible with address
# sanitizer endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
  endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(PCRE2 REQUIRED MODULE)

find_package(SDL2 CONFIG REQUIRED)

add_library(libchess STATIC src/board.c src/piece.c src/move.c src/array.c
  src/pgn.c src/perft.c src/zobrist.c)
target_compile_definitions(libchess PUBLIC PCRE2_CODE_UNIT_WIDTH=8)
target_link_libraries(libchess PUBLIC
  ${PCRE2_LIBRARIES}
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
target_include_directories(libchess PUBLIC src/ ${PCRE2_INCLUDE_DIRS})

add_executable(chess src/main.c)
target_link_libraries(chess PRIVATE libchess)

add_subdirectory(test)
