cmake_minimum_required(VERSION 3.13...3.21)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(etheria
  LANGUAGES C CXX)

project(tier0
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
  src/wizard.hpp
  src/save.hpp
  src/lisp.hpp
  src/scm.hpp
  
  src/main.cpp
  
  src/world.cpp
  src/worldgen.cpp

  src/actor.cpp
  src/culture.cpp

  src/planet.cpp
  src/menus.cpp
  src/help.cpp
  src/wizard.cpp
  src/save.cpp
  src/lisp.cpp
  src/scm.cpp
)

add_library(tier0 SHARED
  tier0/rng.hpp
  tier0/itemtypes.hpp
  tier0/math.hpp
  
  tier0/rng.cpp
  tier0/system.cpp
  tier0/itemtypes.cpp
  tier0/math.cpp

  ${PROTO_SRCS} ${PROTO_HDRS}  
)

set_target_properties(tier0 PROPERTIES OUTPUT_NAME "EtheriaTier0")

target_precompile_headers(tier0 PRIVATE
  tier0/cbase.hpp
)

target_precompile_headers(etheria PRIVATE
  tier0/cbase.hpp
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
find_package(Guile REQUIRED)

target_include_directories(tier0 PUBLIC
  ${LIBNOISE_INCLUDE_DIR}
  ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(tier0 PUBLIC
  libtcod::libtcod
  ${LIBNOISE_LIBRARIES}
  ${Protobuf_LIBRARIES}
)

target_include_directories(etheria PUBLIC
  ${LIBNOISE_INCLUDE_DIR}
  ${Protobuf_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${GUILE_INCLUDE_DIRS}
  tier0
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
  ${GUILE_LIBRARIES}
  tier0
)

project(etheriadit
  LANGUAGES C CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_executable(etheriadit
  dit/main.cpp
  
  # main menu  
  dit/overlook.hpp
  dit/overlook.cpp
  dit/overlook.ui

  # world & rng edit
  dit/world.hpp
  dit/world.cpp
  dit/world.ui

  # meta info edit
  dit/meta.hpp
  dit/meta.cpp
  dit/meta.ui

  # world view
  dit/worldview.hpp
  dit/worldview.cpp
  dit/worldview.ui
)

target_compile_definitions(etheriadit PUBLIC
  ETHERIADIT
)

target_include_directories(etheriadit PUBLIC
  ${Protobuf_INCLUDE_DIRS}
  ${LIBNOISE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  tier0
  src
)

target_link_libraries(etheriadit PRIVATE
  ${Protobuf_LIBRARIES}
  ${LIBNOISE_LIBRARIES}
  tier0
  Qt6::Widgets
  tier0
)

set_target_properties(etheriadit PROPERTIES
  WIN32_EXECUTABLE ON
  MACOSX_BUNDLE ON
)
