cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME GDShare)
set(RUNNER_NAME  OneTimeRunner)

project(${PROJECT_NAME} VERSION 1.0.0)

file(GLOB_RECURSE SOURCES 
    main.cpp
    src/*.cpp
    submodules/nfd/src/nfd_win.cpp
    submodules/nfd/src/nfd_common.c
    src/ext/ZlibHelper.cpp
    src/ext/zip.cpp
)

set_source_files_properties(submodules/nfd/src/nfd_common.c PROPERTIES LANGUAGE CXX)

set(WIN32 ON)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

add_subdirectory(submodules/MinHook)
target_link_libraries(${PROJECT_NAME} submodules/MinHook/Release/minhook.x32)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx/include
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx/kazmath/include
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/third_party/win32/OGLES
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/third_party/win32/zlib
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/win32
    submodules/Cappuccino/template/default-cpp/cappuccino-sdk/incl/extensions
    submodules/MinHook/include
    submodules/nfd/src/include
)

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/libcocos2d.lib
    ${CMAKE_SOURCE_DIR}/submodules/Cappuccino/template/default-cpp/cappuccino-sdk/lib/cappuccino.lib
    ${CMAKE_SOURCE_DIR}/submodules/Cappuccino/template/default-cpp/cappuccino-sdk/lib/libExtensions.lib
)


# runner

add_executable(${RUNNER_NAME} runner.cpp)

set_source_files_properties( runner.cpp PROPERTIES LANGUAGE CXX )
set_source_files_properties( runner.cpp PROPERTIES CXX_STANDARD 17 )
set_source_files_properties( runner.cpp PROPERTIES CXX_STANDARD_REQUIRED ON )

target_link_libraries(${RUNNER_NAME} WtsApi32)

add_definitions(${PROJECT_SOURCE_DIR}/runner.cpp "-DNOMSGBOX")

