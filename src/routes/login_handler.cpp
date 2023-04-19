//
// Created by Xiong Gao on 2023/4/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// for struct evkeyvalq
#include <sys/queue.h>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <signal.h>
#define MYHTTPD_SIGNATURE   "httpd v 0.0.1"

#define LOG_INFO    printf
#define LOG_DBG    printf
#define LOG_ERR    printf

void login_handler(struct evhttp_request *req, void *arg) {
    char output[2048] = "\0";
    char tmp[1024];

    // 获取客户端请求的URI(使用evhttp_request_uri或直接req->uri)
    const char *uri;
    uri = evhttp_request_uri(req);
    sprintf(tmp, "uri=%s\n", uri);
    strcat(output, tmp);

    sprintf(tmp, "uri=%s\n", req->uri);
    strcat(output, tmp);
    // decoded uri
    char *decoded_uri;
    decoded_uri = evhttp_decode_uri(uri);
    sprintf(tmp, "decoded_uri=%s\n", decoded_uri);
    strcat(output, tmp);

    // 解析URI的参数(即GET方法的参数)
    struct evkeyvalq params;
    //将URL数据封装成key-value格式,q=value1, s=value2
    evhttp_parse_query(decoded_uri, &params);
    //得到q所对应的value
    sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "q"));
    strcat(output, tmp);
    //得到s所对应的value
    sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "s"));
    strcat(output, tmp);

    free(decoded_uri);

    // 获取POST方法的数据
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
    sprintf(tmp, "post_data=%s\n", post_data);
    strcat(output, tmp);

    /* 输出到客户端 */

    // HTTP header
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "close");

    // 输出的内容
    struct evbuffer *buf;
    buf = evbuffer_new();
    evbuffer_add_printf(buf, "hello world\n%s\n", output);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}