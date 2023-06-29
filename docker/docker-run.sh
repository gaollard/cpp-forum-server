docker rm ubuntu-gcc-service -f
docker run -itd --name ubuntu-gcc-service -p 8080:8080 gaollard/ubuntu-gcc-service