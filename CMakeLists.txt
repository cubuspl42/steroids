cmake_minimum_required(VERSION 2.8)

set(PROJECT steroids)
set(SERVER steroids_server)
set(CLIENT steroids)

project(${PROJECT})
include(FindPkgConfig)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

set(COMMON_SOURCES
        common/net.h
        common/net.cpp
        common/Packet.h
        common/Socket.h
        common/Socket.cpp
        common/Snake.h
        common/Snake.cpp
        common/vec2.h
        common/vec2.cpp
        common/Color.h
        common/Color.cpp
        common/Arena.h
        common/Arena.cpp
        common/Player.h
        common/Player.cpp
        common/Fruit.h
        common/Fruit.cpp
        common/Config.h
        common/Config.cpp
        common/ServerConfig.h
        common/ServerConfig.cpp)

set(SERVER_SOURCES
        ${COMMON_SOURCES}
        server/main.cpp
        server/Server.h
        server/Server.cpp)

set(CLIENT_SOURCES
        ${COMMON_SOURCES}
        client/main.cpp
        client/Window.h
        client/Window.cpp
        client/Client.h
        client/Client.cpp
        client/Dialog.h
        client/Dialog.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2NET_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS} ${SDL2IMAGE_LIBRARY_DIRS} ${SDL2NET_LIBRARY_DIRS})

add_executable(${SERVER} ${SERVER_SOURCES})
add_executable(${CLIENT} ${CLIENT_SOURCES})

target_link_libraries(${SERVER} ${SDL2NET_LIBRARIES})
target_link_libraries(${CLIENT} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2NET_LIBRARIES})

add_custom_command(TARGET ${CLIENT} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${CLIENT}>)
