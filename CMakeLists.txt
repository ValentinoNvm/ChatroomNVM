cmake_minimum_required(VERSION 3.10)
project(ChatRoom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# per supportare std::thread e aggiungere -pthread
find_package(Threads REQUIRED)

# --- CLIENT APP ---
add_executable(client_app
        ${PROJECT_SOURCE_DIR}/client/client.cpp
        ${PROJECT_SOURCE_DIR}/client/functions.cpp
)
target_include_directories(client_app PRIVATE
        ${PROJECT_SOURCE_DIR}/client
)
target_link_libraries(client_app PRIVATE
        Threads::Threads
)

# --- SERVER APP ---
add_executable(server_app
        ${PROJECT_SOURCE_DIR}/server/server.cpp
        ${PROJECT_SOURCE_DIR}/server/functions.cpp
)
target_include_directories(server_app PRIVATE
        ${PROJECT_SOURCE_DIR}/server
)
target_link_libraries(server_app PRIVATE
        Threads::Threads
)