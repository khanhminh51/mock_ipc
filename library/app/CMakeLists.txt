cmake_minimum_required(VERSION 3.10)

project(processapp)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED)

target_compile_options(${PROJECT_NAME} PUBLIC -fno-elide-constructors)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads rt pthread)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(${PROJECT_NAME} 
    PUBLIC 
    src/ProcessApp.cpp
    src/DataHandler.cpp
    src/Message.cpp
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../../)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION share_lib/app)

install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ProcessApp.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/DataHandler.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Message.h 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share_lib/app/include
)




# cmake_minimum_required(VERSION 3.10)

# project(processapp)
# set(CMAKE_CXX_STANDARD 17)


# add_library(${PROJECT_NAME} STATIC)

# target_compile_options(${PROJECT_NAME} PUBLIC -fno-elide-constructors)
# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads rt)
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_sources(${PROJECT_NAME} PUBLIC ProcessApp.cpp)


# set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# install(TARGETS processapp ARCHIVE DESTINATION share_lib/app)

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ProcessApp.h 
#         DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/app/include 
#        )





# # cmake_minimum_required(VERSION 3.5.0)
# # project(processapp VERSION 0.1.0 LANGUAGES C CXX)

# # set(CMAKE_CXX_STANDARD 17)

# # set(MAIN ProcessApp.cpp)

# # add_library(${PROJECT_NAME} STATIC ${MAIN})

# # target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# # # target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)
# # find_package(Threads REQUIRED)
# # target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads rt)





