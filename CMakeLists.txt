cmake_minimum_required(VERSION 3.1.0)

project(relax)


add_library( Mesh STATICS
    libMesh/mesh.cpp
    libMesh/convert.cpp
)

set(project_sources
    src/main.cpp
)

set(project_headers
    include/vec3f.h
    include/mesh.h
    include/convert.h
)

set(project_compile_flags "-march=native -O2 -ftree-vectorize")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${project_compile_flags}")

add_executable(relax
    ${project_sources}
    ${project_headers}
)

target_link_libraries(relax PRIVATE
    Mesh
)

set_target_properties(relax PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
