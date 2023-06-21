docker rm ubuntu-gcc-service -f
docker run -itd --name ubuntu-gcc-service -p 8081:8081 gaollard/ubuntu-gcc-service