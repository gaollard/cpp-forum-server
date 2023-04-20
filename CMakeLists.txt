cmake_minimum_required(VERSION 3.25)
project(libevent_demo)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib/")

add_executable(libevent_demo main.cpp src/routes/login_handler.cpp include/login_handler.h db.cpp include/db_query.h)

target_link_libraries(libevent_demo event mysqlclient)
