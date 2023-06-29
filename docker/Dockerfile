# 基于哪个镜像进行
FROM ubuntu:22.04

# 复制文件 [复制文件在本地的路径] [文件在容器的路径]
# COPY . .

# 用于设置RUN、CMD、ENTRYPOINT命令的目录
WORKDIR .

# 执行命令
RUN apt-get update

# 安装 git
RUN apt-get install git -y

# 执行命令
RUN apt-get install g++ -y
RUN apt-get install make -y
RUN apt-get install cmake -y

RUN cmake --version

# libevent 安装
RUN apt-get install libssl-dev -y
RUN apt-get install libevent-dev -y

# mysql client 驱动
RUN apt-get install libmysqlclient-dev -y

# sqlcppconn
RUN apt-get install libmysqlcppconn-dev -y

# jsoncpp
RUN apt-get install libjsoncpp-dev -y
RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

# 复制原文件
RUN mkdir www

COPY . www

RUN chmod +x www/scripts/run.sh
RUN chmod +x www/scripts/build.sh

WORKDIR /www

RUN ./scripts/build.sh

ENTRYPOINT ["sh", "/www/scripts/run.sh"]

# 用于设置与主机相连的端口号
EXPOSE 8081