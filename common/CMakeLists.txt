project(common)
file(GLOB sources
        *.c
        *.cpp
        *.cxx)

file(GLOB headers
        include/*.h
        include/*.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)