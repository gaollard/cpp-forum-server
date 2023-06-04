## forum-powered-by-cpp

- libevent
- mysqlclient
- rapidjson
- cppjson
- redis client
- boost
- 数据库连接池
- https://github.com/sewenew/redis-plus-plus
- xml
- 正则

## API 列表
### 1、query_user_list 查询用户列表
```shell
curl -X POST localhost:8080/query_user_list
```

### 2、query_user_list 登录
```shell
curl -X POST localhost:8080/account/login \
--data-raw '{"account": "admin", "password": "123456"}'
```

### 3、register_action 注册
```shell
curl -X POST localhost:8080/account/register \
--data-raw '{"account": "admin", "password": "123456"}'
```
