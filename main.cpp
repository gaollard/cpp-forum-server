#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <signal.h>
#include "./include/_mysql.h"
#include "./src/db/mysql_wrapper.h"
#include <iostream>
#include <vector>;

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

struct user {
    int id;
    char name[30];
} stgirls;

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

void my_query() {

}

int db_test_01 () {
    connection conn; // 数据库连接类

    // 登录数据库，返回值：0-成功，其它-失败。
    // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
    if (conn.connecttodb("127.0.0.1,root,123456,test,3306","utf-8") != 0)
    {
        printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
    }

    sqlstatement stmt(&conn); // 操作SQL语句的对象。

    // 准备查询表的SQL语句。
    stmt.prepare("select id,name from user where id>=:1");
    int min_id = 15;
    stmt.bindin(1,&min_id);

    // 为SQL语句绑定输出变量的地址，bindout 方法不需要判断返回值。
    stmt.bindout(1,&stgirls.id);
    stmt.bindout(2, stgirls.name,30);

    // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    // 失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中。
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
    }
    // db 查询测试
    std::vector<user> users;

    // 本程序执行的是查询语句，执行stmt.execute()后，将会在数据库的缓冲区中产生一个结果集。
    while (1)
    {
        memset(&stgirls,0,sizeof(stgirls)); // 先把结构体变量初始化。

        // 从结果集中获取一条记录，一定要判断返回值，0-成功，1403-无记录，其它-失败。
        // 在实际开发中，除了0和1403，其它的情况极少出现。
        if (stmt.next() !=0) break;

        users.push_back(stgirls);

        // 把获取到的记录的值打印出来。
        printf("id=%ld,name=%s,weight=%.02f,btime=%s\n",stgirls.id,stgirls.name);
    }

    // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
    printf("本次查询了 girls 表%ld条记录。\n", stmt.m_cda.rpc);
    printf("本次查询了 girls 表%ld条记录。\n", users.size());

    for(std::vector<user>::iterator it = users.begin(); it != users.end();it++) {
        printf("id=%ld, name=%s \n",it->id,it->name);
    }
}

void db_test_02 () {
    db::DbConf::Conf_ db_conf;
    db_conf.host = "175.178.48.236";
    db_conf.port = 3306;
    db_conf.db_name = "test";
    db_conf.user = "gaollard";
    db_conf.password = "gaoxiong123.";
    db_conf.conn_pool_size = 10;
    db::MysqlWrapper wrapper(db::DbConf::DB_CONN_RW);

    int res = wrapper.connect(db_conf);

    std::vector< std::map<std::string, std::string> > map;
    wrapper.SelectQuery("select * from user", map);

    std::cout << map.at(0).at("name");
    std::cout << map.at(0).at("id");

    std::cout << res << std::endl;
}

int main(int argc, char *argv[]) {

    // db_query();
    // db_test_01();
    db_test_02();
    return 0;

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