FROM alpine:3.7
RUN apk update && apk add -y zip sudo
RUN adduser -u 1000 -S user && \
    echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/default

