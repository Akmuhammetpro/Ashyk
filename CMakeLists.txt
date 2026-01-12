cmake_minimum_required(VERSION 3.26)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)


project(Ashyk)
option(ASHYK_ENABLE_RAYLIB "Build raylib graphical target" ON)
# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME Ashyk)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(headers)

if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(STATUS "Disabling raylib on macOS with GCC (GLFW/Cocoa requires clang).")
  set(ASHYK_ENABLE_RAYLIB OFF CACHE BOOL "" FORCE)
endif()

if(ASHYK_ENABLE_RAYLIB)
  include(FetchContent)
  FetchContent_Declare(
      raylib
      GIT_REPOSITORY https://github.com/raysan5/raylib.git
      GIT_TAG        5.0
  )
  FetchContent_GetProperties(raylib)
  if(NOT raylib_POPULATED)
    FetchContent_Populate(raylib)
    set(raylib_cmake "${raylib_SOURCE_DIR}/CMakeLists.txt")
    if(EXISTS "${raylib_cmake}")
      file(READ "${raylib_cmake}" raylib_cmake_contents)
      string(REGEX REPLACE "cmake_minimum_required\\(VERSION[ \t]+[0-9.]+\\)"
                           "cmake_minimum_required(VERSION 3.10)"
                           raylib_cmake_contents
                           "${raylib_cmake_contents}")
      file(WRITE "${raylib_cmake}" "${raylib_cmake_contents}")
    endif()
  endif()
  add_subdirectory("${raylib_SOURCE_DIR}" "${raylib_BINARY_DIR}")
endif()

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################

add_executable(Ashyk
    src/Asik.cpp
    src/Player.cpp
    src/Throw.cpp
    src/Game.cpp
    src/GameEngine.cpp
    src/RaylibGameEngine.cpp
    main.cpp
)
set(ASHYK_TARGETS Ashyk)

if(ASHYK_ENABLE_RAYLIB)
  target_link_libraries(Ashyk PRIVATE raylib)
endif()

if(ASHYK_ENABLE_RAYLIB)
  add_executable(AshykRaylib
      src/Asik.cpp
      src/Player.cpp
      src/Throw.cpp
      src/Game.cpp
      src/GameEngine.cpp
      src/main_raylib.cpp
  )
  target_link_libraries(AshykRaylib PRIVATE raylib)
  list(APPEND ASHYK_TARGETS AshykRaylib)
endif()

# Apply the same compiler flags/sanitizers to both executables
set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${ASHYK_TARGETS})

###############################################################################

# Install console build to bin/
install(TARGETS Ashyk DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME Ashyk)
copy_files(DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME Ashyk)
if(ASHYK_ENABLE_RAYLIB)
  copy_files(DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME AshykRaylib)
endif()
