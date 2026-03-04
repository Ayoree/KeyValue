cmake_minimum_required(VERSION 3.20)
project(KeyValueServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

cmake_policy(SET CMP0167 OLD)
find_package(Boost REQUIRED COMPONENTS system)
find_package(TBB REQUIRED)

add_compile_options(
    -Wall
    -Werror
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/server/*.cpp")
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "src/client/*.cpp")

add_executable(server WIN32 ${SERVER_SOURCES})
add_executable(client WIN32 ${CLIENT_SOURCES})

target_precompile_headers(server PRIVATE "src/server/pch.hpp")
target_precompile_headers(client PRIVATE "src/client/pch.hpp")

target_link_libraries(server PRIVATE TBB::tbb)
foreach(tgt IN ITEMS server client)
    target_link_libraries(${tgt} PRIVATE Boost::system)
    target_compile_definitions(${tgt} PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:RELEASE>
    )
endforeach()

add_custom_command(
    TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:server>/res
    COMMENT "Copying `${CMAKE_SOURCE_DIR}/res` -> `$<TARGET_FILE_DIR:server>/res`"
)
