//
// Created by Xiong Gao on 2023/4/20.
//

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/utils/send.h"
#include "../db/db_conn_pool.h"

typedef struct user {
    int id;
    char name[30];
} user;

void db_test_02 () {
    // 查询
    std::vector<std::map<std::string, std::string> > map;
    db::DbConnPool::getInstance()->GetConn(db::DbConf::DbConnType::DB_CONN_RW)->SelectQuery("select * from user", map);
    std::cout << map.size() << std::endl;
    std::cout << "  ID: " << map.at(0).at("id") << std::endl;
    std::cout << "Name: " << map.at(0).at("name") << std::endl;
}

void query_user_list(struct evhttp_request *req, void *arg) {
    char tmp[1024];
    std::vector<std::map<std::string, std::string> > list;
    db::DbConnPool::getInstance()->GetConn(db::DbConf::DbConnType::DB_CONN_RW)->SelectQuery("select * from user", list);
    sprintf(tmp, "{\"total\": %d}", list.size());
    send(req, tmp);
}