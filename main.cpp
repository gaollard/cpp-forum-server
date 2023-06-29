#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <signal.h>
#include "./src/db/mysql_wrapper.h"
#include "src/db/db_conn_pool.h"

#define LOG_INFO    printf

#include "src/routes/login_handler.h"
#include "src/routes/get_user_info_handler.h"
#include "src/routes/generic_handler.h"
#include "src/routes/query_user_list.h"
#include "src/routes/register_action.h"

void show_help() {
    char *help = "http://localhost:8080\n"
                 "-l <ip_addr> interface to listen on, default is 0.0.0.0\n"
                 "-p <num>     port number to listen on, default is 1984\n"
                 "-d           run as a deamon\n"
                 "-t <second>  timeout for a http request, default is 120 seconds\n"
                 "-h           print this help and exit\n"
                 "\n";
    fprintf(stderr,"%s",help);
}

// 当向进程发出SIGTERM/SIGHUP/SIGINT/SIGQUIT的时候，终止event的事件侦听循环
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
            event_loopbreak();  //终止侦听event_dispatch()的事件侦听循环，执行之后的代码
            break;
    }
}

void init_db() {
    // 连接配置
    db::DbConf::Conf_ db_conf;
    db_conf.host = "175.178.48.236";
    db_conf.port = 3306;
    db_conf.db_name = "test";
    db_conf.user = "gaollard";
    db_conf.password = "gaoxiong123.";
    db_conf.conn_pool_size = 10;

    // 创建连接池
    db::DbConnPool::getInstance()->init(db_conf, db_conf);
}

int main(int argc, char *argv[]) {

    init_db();

    //自定义信号处理函数
    signal(SIGHUP   , signal_handler);
    signal(SIGTERM  , signal_handler);
    signal(SIGINT   , signal_handler);
    signal(SIGQUIT  , signal_handler);

    //默认参数
    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_daemon = 0;
    int httpd_option_timeout = 120; //in seconds

    LOG_INFO("http server start %s:%d\n", httpd_option_listen, httpd_option_port);

    //获取参数
    int c;
    while ((c = getopt(argc, argv, "l:p:dt:h")) != -1) {
        switch (c) {
            case 'l' :
                httpd_option_listen = optarg;
                break;
            case 'p' :
                httpd_option_port = atoi(optarg);
                break;
            case 'd' :
                httpd_option_daemon = 1;
                break;
            case 't' :
                httpd_option_timeout = atoi(optarg);
                break;
            case 'h' :
            default :
                show_help();
                exit(EXIT_SUCCESS);
        }
    }

    //判断是否设置了-d，以daemon运行
    if (httpd_option_daemon) {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            //生成子进程成功，退出父进程
            exit(EXIT_SUCCESS);
        }
    }

    // 初始化 event API
    event_init();

    // 创建一个http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);
//    evhttp_request_set_error_cb(httpd, )

    // 指定 generic callback
    evhttp_set_gencb(httpd, generic_handler, NULL);

    // 登录
    evhttp_set_cb(httpd, "/account/user_login", login_handler, NULL);

    // 注册
    evhttp_set_cb(httpd, "/account/register", register_action, NULL);

    // 查询用户信息
    evhttp_set_cb(httpd, "/get_user_info", get_user_info_handler, NULL);

    // 查询用户列表
    evhttp_set_cb(httpd, "/query_user_list", query_user_list, NULL);

    //开始事件监听，分发
    event_dispatch();

    //释放资源
    evhttp_free(httpd);
    return 0;
}