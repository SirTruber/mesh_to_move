cmake_minimum_required(VERSION 3.1.0)

project(relax)


add_library(libMesh STATIC
    libMesh/vec3f.cpp
    libMesh/mesh.cpp
)

set(project_sources
    src/main.cpp
)

set(project_headers
    include/vec3f.h
    include/mesh.h
)

set(project_compile_flags "-O2")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${project_compile_flags}")

add_executable(relax
    ${project_sources}
    ${project_headers}
)

target_link_libraries(relax PRIVATE
    libMesh
)

set_target_properties(relax PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
