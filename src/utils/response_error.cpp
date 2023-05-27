//
// Created by Xiong Gao on 2023/5/27.
//

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/utils/send.h"
#include "json/json.h"

void response_error(struct evhttp_request *req, char *arg) {
    Json::Value root;
    Json::Value data;

    root["err_msg"] = arg;
    root["err_no"] = 1;
    root["data"] = data;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string json_string = Json::writeString(builder, root);

    send(req, (char*) json_string.c_str());
}