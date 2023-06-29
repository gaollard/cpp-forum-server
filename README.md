# cpp-forum-server

```shell
# 构建镜像
sh docker/docker-build.sh

# 启动服务
sh docker/docker-run.sh
```

## 技术选型

- libevent
- mysqlclient
- rapidjson
- cppjson
- redis client https://github.com/sewenew/redis-plus-plus
- 数据库连接池

## API 列表
### 1. query_user_list 查询用户列表
```shell
curl -X POST localhost:8080/query_user_list
```

### 2. query_user_list 登录
```shell
curl -X POST localhost:8080/account/login \
--data-raw '{"account": "admin", "password": "123456"}'
```

### 3. register_action 注册
```shell
curl -X POST localhost:8080/account/register \
--data-raw '{"account": "admin", "password": "123456"}'
```
