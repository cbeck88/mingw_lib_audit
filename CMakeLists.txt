cmake_minimum_required (VERSION 2.8.11)
project (al_test)

#add_subdirectory(../openal-soft-1.16.0 "${CMAKE_CURRENT_BINARY_DIR}/openal")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(epoxy REQUIRED)

MESSAGE(STATUS "Includes")
MESSAGE(STATUS "PNG: " ${PNG_INCLUDE_DIRS})
MESSAGE(STATUS "Freetype: " ${FREETYPE_INCLUDE_DIRS})
MESSAGE(STATUS "OPENAL: " ${OPENAL_INCLUDE_DIR})
MESSAGE(STATUS "OPENGL: " ${OPENGL_INCLUDE_DIR})
MESSAGE(STATUS "SDL2: " ${SDL2_INCLUDE_DIR})
MESSAGE(STATUS "epoxy: " ${EPOXY_INCLUDE_DIR})

MESSAGE(STATUS "Bins")
MESSAGE(STATUS "PNG: " ${PNG_LIBRARIES})
MESSAGE(STATUS "FREETYPE: " ${FREETYPE_LIBRARIES})
MESSAGE(STATUS "OPENGL: " ${OPENGL_LIBRARY})
MESSAGE(STATUS "SDL2: " ${SDL2_LIBRARY})
MESSAGE(STATUS "epoxy: " ${EPOXY_LIBRARY})


if (UNIX)
  set(CMATH_LIB m)
else()
  set(CMATH_LIB)
endif()


add_executable (al_test openal_playback.cpp)
target_include_directories (al_test PUBLIC ${OPENAL_INCLUDE_DIR})
target_link_libraries(al_test LINK_PUBLIC ${OPENAL_LIBRARY})

add_executable (freetype_test freetype_rotate.c)
target_include_directories (freetype_test PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(freetype_test LINK_PUBLIC ${FREETYPE_LIBRARIES} ${CMATH_LIB})

add_executable (triangle_test opengl_triangle_test.cpp)
target_include_directories (triangle_test PUBLIC ${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(triangle_test LINK_PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARY})

add_executable (triangle_test_epoxy opengl_triangle_test.cpp)
target_include_directories (triangle_test_epoxy PUBLIC ${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${EPOXY_INCLUDE_DIR})
target_link_libraries(triangle_test_epoxy LINK_PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARY} ${EPOXY_LIBRARY})
target_compile_definitions (triangle_test_epoxy PUBLIC -DUSE_EPOXY)

add_executable (triangle_test2 opengl_triangle_test2.cpp)
target_include_directories (triangle_test2 PUBLIC ${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
target_link_libraries(triangle_test2 LINK_PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARY})

add_executable (triangle_test2_epoxy opengl_triangle_test2.cpp)
target_include_directories (triangle_test2_epoxy PUBLIC ${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${EPOXY_INCLUDE_DIR})
target_link_libraries(triangle_test2_epoxy LINK_PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARY} ${EPOXY_LIBRARY})
target_compile_definitions (triangle_test2_epoxy PUBLIC -DUSE_EPOXY)

add_executable (pngtest pngtest.c)
target_include_directories (pngtest PUBLIC ${PNG_INCLUDE_DIRS})
target_link_libraries(pngtest LINK_PUBLIC ${PNG_LIBRARIES})
