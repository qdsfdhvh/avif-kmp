cmake_minimum_required(VERSION 3.4.1)
project(avif_jvm_jni)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

set(PROJECT_SRC_DIR ${CMAKE_SOURCE_DIR}/../../src)
set(PROJECT_LIB_DIR ${CMAKE_SOURCE_DIR}/../../darwin)

add_library(dav1d STATIC IMPORTED)
add_library(avif STATIC IMPORTED)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${PROJECT_LIB_DIR}/build/darwin/libdav1d.a)
set_target_properties(avif PROPERTIES IMPORTED_LOCATION ${PROJECT_LIB_DIR}/build/darwin/libavif.a)

include_directories(${PROJECT_LIB_DIR}/libavif/include)
include_directories(${PROJECT_SRC_DIR}/jniMain/cpp)
include_directories(skia/include skia)

file(GLOB_RECURSE sources
        ${PROJECT_SRC_DIR}/jvmMain/cpp/*.cpp
        ${PROJECT_SRC_DIR}/jniMain/cpp/*.cpp)
add_library(avif-jvm SHARED ${sources})

target_link_libraries(avif-jvm dav1d avif)
