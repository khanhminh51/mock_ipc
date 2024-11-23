cmake_minimum_required(VERSION 3.10)

project(server)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED)

target_compile_options(${PROJECT_NAME} PUBLIC -fno-elide-constructors)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads rt pthread)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp)
target_sources(${PROJECT_NAME} 
    PUBLIC 
    src/Server.cpp
    src/Message.cpp
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../../)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION share_lib/server)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Server.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Message.h 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/server/include)




# cmake_minimum_required(VERSION 3.10)

# project(server)
# set(CMAKE_CXX_STANDARD 17)

# add_library(${PROJECT_NAME} STATIC)

# target_compile_options(${PROJECT_NAME} PUBLIC -fno-elide-constructors)
# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads rt)
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_sources(${PROJECT_NAME} PUBLIC Server.cpp)

# set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# install(TARGETS server ARCHIVE DESTINATION share_lib/server)

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Server.h 
#         DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/server/include 
#        )





# # cmake_minimum_required(VERSION 3.5.0)
# # project(server VERSION 0.1.0 LANGUAGES C CXX)

# # set(CMAKE_CXX_STANDARD 17)

# # set(MAIN Server.cpp)

# # # add_executable(${PROJECT_NAME} ${MAIN})
# # add_library(${PROJECT_NAME} STATIC ${MAIN})

# # target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# # # target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)
# # find_package(Threads REQUIRED)
# # target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads rt)



