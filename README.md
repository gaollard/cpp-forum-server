## forum-powered-by-cpp

- libevent
- mysqlclient
- rapidjson
- cppjson
- boost
- 数据库连接池
- https://github.com/sewenew/redis-plus-plus

## API 列表
### query_user_list 查询用户列表
```shell
curl -X POST localhost:8080/query_user_list
```

### query_user_list 查询用户列表
```shell
curl -X POST localhost:8080/login \
--data-raw '{"account": "admin", "password": "123456"}'
```