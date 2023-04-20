#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/queue.h>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <signal.h>
#include "./include/db_query.h"

#define LOG_INFO    printf
#define LOG_DBG    printf
#define LOG_ERR    printf

#include "include/routes/login_handler.h";
#include "include/routes/get_user_info_handler.h"
#include "include/routes/generic_handler.h"

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

int main(int argc, char *argv[]) {

//    db_query();

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

    /* 使用libevent创建HTTP Server */

    //初始化event API
    event_init();

    //创建一个http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);

    // 指定 generic callback
    evhttp_set_gencb(httpd, generic_handler, NULL);

    // 也可以为特定的URI指定callback
    evhttp_set_cb(httpd, "/login", login_handler, NULL);

    // 也可以为特定的URI指定callback
    evhttp_set_cb(httpd, "/get_user_info", get_user_info_handler, NULL);

    //开始事件监听，分发
    event_dispatch();

    //释放资源
    evhttp_free(httpd);
    return 0;
}