cmake_minimum_required(VERSION 3.9)
project(cpp-forum-server)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib/")

add_executable(main main.cpp src/routes/query_user_list.h src/routes/query_user_list.cpp src/routes/login_handler.cpp src/routes/get_user_info_handler.cpp src/routes/login_handler.h db.cpp src/routes/generic_handler.cpp src/utils/send.cpp src/utils/_mysql.cpp src/db/mysql_wrapper.cpp src/db/db_conn_pool.cpp src/db/db_proxy_4_transaction.cpp src/routes/query_user_list.cpp src/routes/register_action.h src/routes/register_action.cpp src/utils/response_error.cpp src/utils/response_error.h include/db_query.h include/db_query.h)

target_link_libraries(main event mysqlclient jsoncpp)
