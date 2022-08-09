cmake_minimum_required(VERSION 3.13...3.21)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(etheria
  LANGUAGES C CXX)

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
  proto/etheriasaves.proto
)

# bin/etheria, ENGINE
add_executable(etheria
  src/cbase.hpp
  src/world.hpp
  src/actor.hpp
  src/culture.hpp
  src/planet.hpp
  src/menus.hpp
  src/help.hpp
  src/rng.hpp
  src/wizard.hpp
  src/save.hpp
  
  src/main.cpp
  src/system.cpp
  
  src/world.cpp
  src/worldgen.cpp

  src/itemtypes.cpp
  src/actor.cpp
  src/culture.cpp
  src/planet.cpp
  src/menus.cpp
  src/help.cpp
  src/rng.cpp
  src/wizard.cpp
  src/save.cpp

  ${PROTO_SRCS} ${PROTO_HDRS}
)

target_precompile_headers(etheria PRIVATE
  src/cbase.hpp
)

target_compile_features(etheria PRIVATE
  cxx_std_17
)

if(MSVC)
  target_compile_options(etheria PRIVATE /utf-8 /W4)  
else()
  target_compile_options(etheria PRIVATE -Wall -Wextra)
endif()

if (EMSCRIPTEN)
    # Attach data folder to Emscripten builds.
    target_link_options(etheria PRIVATE --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/data@data")
    # Set output to html to generate preview pages, otherwise you'll have to make your own html.
    set_target_properties(
        etheria
        PROPERTIES
            SUFFIX ".html"
    )
endif()

set(BUILD_SHARED_LIBS ${WIN32})
add_subdirectory("libtcod")

find_package(SDL2 CONFIG REQUIRED)
find_package(LibNoise REQUIRED)

target_include_directories(etheria PUBLIC
  ${LIBNOISE_INCLUDE_DIR}
  ${Protobuf_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}  
)

target_compile_definitions(etheria PUBLIC
  WIZARD
)

target_link_libraries(etheria PRIVATE
  SDL2::SDL2
  SDL2::SDL2main
  libtcod::libtcod
  ${LIBNOISE_LIBRARIES}
  ${Protobuf_LIBRARIES}
)
