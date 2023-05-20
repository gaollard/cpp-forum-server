//
// Created by Xiong Gao on 2023/4/20.
//

#include "json/json.h"
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
    std::vector<std::map<std::string, std::string> > s_list;
    db::DbConnPool::getInstance()->GetConn(db::DbConf::DbConnType::DB_CONN_RW)->SelectQuery("select * from user", s_list);

    Json::Value root;
    Json::Value list;
    root["count"] = (int)s_list.size();

    for (std::vector<std::map<std::string, std::string>>::iterator it = s_list.begin(); it != s_list.end(); it++)
    {
        Json::Value data;
        data["name"] = it->at("name");
        data["age"] = atoi(it->at("id").c_str());
        list.append(data);
    }
    root["list"] = list;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string json_string = Json::writeString(builder, root);

    send(req, json_string.data());
}