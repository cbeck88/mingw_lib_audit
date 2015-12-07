cmake_minimum_required (VERSION 2.8.11)
project (al_test)

#add_subdirectory(../openal-soft-1.16.0 "${CMAKE_CURRENT_BINARY_DIR}/openal")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)

MESSAGE(STATUS "Freetype: " ${FREETYPE_INCLUDE_DIRS})
MESSAGE(STATUS "OPENAL: " ${OPENAL_INCLUDE_DIR})

add_executable (al_test openal_playback.cpp)
target_include_directories (al_test PUBLIC ${OPENAL_INCLUDE_DIR})
target_link_libraries(al_test LINK_PUBLIC ${OPENAL_LIBRARY})

add_executable (freetype_test freetype_rotate.c)
target_include_directories (freetype_test PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(freetype_test LINK_PUBLIC ${FREETYPE_LIBRARIES})

