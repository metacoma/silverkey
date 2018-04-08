#FROM golang:1.9.5-alpine
#FROM blang/golang-alpine:latest
FROM ubuntu:xenial

RUN apt-get update && apt-get install -y \
  git                                    \
  build-essential                        \
  libwebkit2gtk-4.0                      \
  libxtst-dev libpng++-dev               \
  xcb libxcb-xkb-dev x11-xkb-utils libx11-xcb-dev libxkbcommon-x11-dev \
  libxtst-dev libpng++-dev               \
  xorg-dev                               \
  libx11-dev                             \
  libc6-dev                              \
  xsel                                   \
  software-properties-common             \
  xclip

RUN add-apt-repository ppa:gophers/archive
RUN apt-get update && apt-get install -y golang-1.9 golang-1.9-go


ENV PATH="/usr/lib/go-1.9/bin:${PATH}"
#RUN apk add --update git alpine-sdk webkit2gtk-dev
RUN go get github.com/octoblu/go-simple-etcd-client/etcdclient
RUN go get github.com/wneo/jlfuzzy
RUN go get github.com/zserge/webview
RUN go get github.com/go-vgo/robotgo
RUN go get -u github.com/jteeuwen/go-bindata/...
RUN go get github.com/hidez8891/shm


CMD ["/bin/sh"]
ENTRYPOINT [ "/bin/sh" ]
