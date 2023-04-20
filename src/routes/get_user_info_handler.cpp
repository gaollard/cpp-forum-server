//
// Created by Xiong Gao on 2023/4/19.
//

#include <stdio.h>
#include <string.h>

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/utils/send.h"

void get_user_info_handler(struct evhttp_request *req, void *arg) {
    char output[2048] = "\0";
    char tmp[1024];

    // 获取POST方法的数据
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
    sprintf(tmp, "post_data=%s\n", post_data);
    strcat(output, tmp);

    // 输出的内容
    char content[] = "{\"user\":\"name\"}";
    send(req, content);
}