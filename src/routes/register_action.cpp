//
// Created by Xiong Gao on 2023/4/19.
//

#include <iostream>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/utils/send.h"
#include "json/json.h"
#include "../db/db_conn_pool.h"
#include "string"

void register_action(struct evhttp_request *req, void *arg) {
    // 获取POST方法的数据
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);

    Json::Reader jsReader;
    Json::Value jsonData;

    if (!jsReader.parse(post_data , jsonData))
    {
        std::cout << "parse body error" << std::endl;
        return;
    }

    std::stringstream sql;
    sql << "insert into user(name, password) value";
    sql << "('" << jsonData["account"].asString() << "', '" << jsonData["password"].asString() << "')";
    sql << ";";

    Json::Value root;
    Json::Value data;
    int rows = db::DbConnPool::getInstance()->GetConn(db::DbConf::DbConnType::DB_CONN_RW)->UpdateDb(sql.str());

    if (rows == 1) {
        std::cout << "注册成功" << std::endl;
        std::map<std::string, std::string> s_map;
        std::stringstream sql_query;
        sql_query << "select * from user where name = '" <<  jsonData["account"].asString() << "';";
        int res = db::DbConnPool::getInstance()->GetConn(db::DbConf::DbConnType::DB_CONN_RW)->SelectSingleLineQuery(sql_query.str(), s_map);
        if (res == 0) {
            root["err_no"] = 0;
            data["id"] = atoi(s_map.at("id").c_str());
            data["name"] = s_map.at("name");
            data["password"] = s_map.at("password");
            data["token"] = "123456";
            root["data"] = data;
        } else {
            root["err_no"] = 1;
            root["data"] = Json::nullValue;
        }
    } else {
        root["err_no"] = 1;
        root["data"] = Json::nullValue;
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string json_string = Json::writeString(builder, root);

    send(req, json_string.data());
}