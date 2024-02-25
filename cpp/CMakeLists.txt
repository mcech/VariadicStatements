cmake_minimum_required(VERSION 3.25)
project(CDatabase)

set(CMAKE_CXX_STANDARD 20)
add_executable(${CMAKE_PROJECT_NAME} main.cpp Connection.cpp Result.cpp SQLException.cpp)

find_package(ODBC REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}
    ODBC::ODBC
    /usr/lib/x86_64-linux-gnu/odbc/libmaodbc.so
)
