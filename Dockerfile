FROM debian:bookworm

RUN apt-get update && \
    apt-get install -y \
    make \
    build-essential \
    valgrind \
    traceroute \
    tcpdump

    

WORKDIR /app
