project(lab5)
add_executable(${PROJECT_NAME} main.cpp)

add_library(${PROJECT_NAME}_lib INTERFACE)
target_include_directories(${PROJECT_NAME}_lib INTERFACE include)

include(FindPkgConfig)
pkg_search_module(gvc REQUIRED libgvc libcgraph libcdt)
link_directories(${gvc_LIBRARY_DIRS})
include_directories(${gvc_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} common ${PROJECT_NAME}_lib ${gvc_LIBRARIES})