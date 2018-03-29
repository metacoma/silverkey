FROM golang:1.9.4-alpine
RUN apk add --update git
WORKDIR /go/src/app
COPY src .

RUN go get github.com/octoblu/go-simple-etcd-client/etcdclient
RUN go get github.com/wneo/jlfuzzy
RUN go install -v ./...
RUN go build
#CMD ["app"]

ENTRYPOINT [ "app" ]
