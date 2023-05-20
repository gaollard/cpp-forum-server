//
// Created by Xiong Gao on 2023/4/19.
//

#include <iostream>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/utils/send.h"
#include "json/json.h"

void login_handler(struct evhttp_request *req, void *arg) {
    // 获取POST方法的数据
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);

    Json::Reader jsReader;
    Json::Value jsonData;

    if (!jsReader.parse(post_data , jsonData))
    {
        std::cout << "parse body error" << std::endl;
        return;
    }

    Json::Value user_info;
    user_info["id"] = 1;
    user_info["account"] = jsonData["account"].asString();
    user_info["password"] = jsonData["password"].asString();
    user_info["token"] = "9527";

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string json_string = Json::writeString(builder, user_info);

    send(req, json_string.data());
}