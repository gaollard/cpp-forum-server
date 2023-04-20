cmake_minimum_required(VERSION 3.25)
project(libevent_demo)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib/")

add_executable(libevent_demo main.cpp src/routes/login_handler.cpp src/routes/get_user_info_handler.cpp include/routes/login_handler.h db.cpp include/db_query.h include/common.h include/routes/get_user_info_handler.h include/routes/generic_handler.h src/routes/generic_handler.cpp include/utils/send.h src/utils/send.cpp)

target_link_libraries(libevent_demo event mysqlclient)
