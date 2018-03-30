#FROM golang:1.9.4-alpine
FROM golang:1.9.3-alpine
RUN apk add --update git gtk+3.0-dev
WORKDIR /go/src/app
COPY src .

RUN go get github.com/octoblu/go-simple-etcd-client/etcdclient
RUN go get github.com/wneo/jlfuzzy
RUN go install -v ./...
RUN go build
#CMD ["app"]

ENTRYPOINT [ "app" ]
