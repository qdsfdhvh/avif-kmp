cmake_minimum_required(VERSION 3.11.0)
project(avif_android_jni)

#add_subdirectory(${CMAKE_SOURCE_DIR}/../../darwin/libavif libavif)

set(PROJECT_SRC_DIR ${CMAKE_SOURCE_DIR}/../../src)
set(PROJECT_LIB_DIR ${CMAKE_SOURCE_DIR}/../../darwin)

add_library(dav1d STATIC IMPORTED)
add_library(avif STATIC IMPORTED)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${PROJECT_LIB_DIR}/build/android/${ANDROID_ABI}/libdav1d.a)
set_target_properties(avif PROPERTIES IMPORTED_LOCATION ${PROJECT_LIB_DIR}/build/android/${ANDROID_ABI}/libavif.a)

include_directories(${PROJECT_LIB_DIR}/libavif/include)
include_directories(${PROJECT_SRC_DIR}/jniMain/cpp)

file(GLOB_RECURSE sources
        ${PROJECT_SRC_DIR}/androidMain/cpp/*.cpp
        ${PROJECT_SRC_DIR}/jniMain/cpp/*.cpp)
add_library(avif-android SHARED ${sources})

target_link_libraries(avif-android jnigraphics dav1d avif log)
