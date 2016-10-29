cmake_minimum_required(VERSION 3.6)
project(CreatureBrains)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set( CMAKE_EXE_LINKER_FLAGS "-llapack -lclapack -lcblas")

file(GLOB_RECURSE SOURCE_FILES
        "src/**.cpp"
        )

find_package( BLAS REQUIRED )

execute_process(COMMAND echo bar OUTPUT_VARIABLE FOO)
include_directories(include)

include_directories(/usr/local/opt/openblas/include)
link_directories(/usr/local/opt/openblas/lib)

add_executable(CreatureBrains ${SOURCE_FILES})