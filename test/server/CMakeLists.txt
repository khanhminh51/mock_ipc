cmake_minimum_required(VERSION 3.10)

project(server)

add_executable(${PROJECT_NAME})

find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/server/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../share_lib/server/libserver.so Threads::Threads rt)

target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp)


# cmake_minimum_required(VERSION 3.5.0)
# project(server LANGUAGES CXX)

# # Tạo file thực thi
# add_executable(${PROJECT_NAME})

# target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/server/server)
# target_link_libraries(${PROJECT_NAME} PUBLIC /usr/local/lib/libserver.a)
# target_sources(${PROJECT_NAME} PUBLIC server.cpp)


# cmake_minimum_required(VERSION 3.10)
# project(server)

# # Đường dẫn đến các file thư viện
# set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../../library)

# # Include thư viện Server
# add_library(ServerLib STATIC
#     ${LIBRARY_DIR}/server/Server.cpp
# )

# target_include_directories(ServerLib PUBLIC
#     ${LIBRARY_DIR}/server
# )

# # Include thư viện ProcessApp
# # add_library(AppLib STATIC
# #     ${LIBRARY_DIR}/app/ProcessApp.cpp
# # )

# # target_include_directories(AppLib PUBLIC
# #     ${LIBRARY_DIR}/app
# # )

# # Tạo file thực thi test
# add_executable(server server.cpp)

# # Link các thư viện vào file thực thi
# target_link_libraries(server ServerLib pthread rt)
# # target_link_libraries(server ServerLib AppLib pthread rt)


