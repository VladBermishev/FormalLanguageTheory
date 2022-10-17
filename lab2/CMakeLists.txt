project(lab2)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} common)
target_include_directories(${PROJECT_NAME} PUBLIC include)