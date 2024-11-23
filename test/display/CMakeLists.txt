cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(display)

add_executable(${PROJECT_NAME})
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/app/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/app/libprocessapp.so pthread CURL::libcurl)

target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/display.cpp)