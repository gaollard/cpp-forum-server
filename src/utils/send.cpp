//
// Created by Xiong Gao on 2023/4/20.
//

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../../include/common.h"

void send(struct evhttp_request *req, char* content) {
    // HTTP header
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "application/json;charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "close");

    struct evbuffer *buf;
    buf = evbuffer_new();
    evbuffer_add_printf(buf, content);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}