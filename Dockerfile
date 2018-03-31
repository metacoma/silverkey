FROM magj/go-ui-crossbuild
WORKDIR /go/src/
COPY src /go/src/silverkey-golang-client
ENV CGO_ENABLED=1
ENV GOARCH=amd64
RUN go get -d github.com/andlabs/ui
#workaround
RUN cp /tmp/libui_linux_amd64.a /go/src/github.com/andlabs/ui/libui_linux_amd64.a
RUN go get github.com/andlabs/ui
RUN go get github.com/octoblu/go-simple-etcd-client/etcdclient
RUN go get github.com/wneo/jlfuzzy
WORKDIR /go/src/silverkey-golang-client
RUN go install -v ./...
RUN GOOS=linux CC=clang CXX=clang++ go build -o silverkey_linux silverkey-client.go
RUN GOOS=windows CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ go build -o silverkey-client_windows.exe -ldflags "-H=windowsgui -extldflags=-s" silverkey-client.go
RUN GOOS=darwin CGO_LDFLAGS_ALLOW="-mmacosx-version-min.*" CC=o64-clang CXX=o64-clang++ go build -o silverkey_darwin.app silverkey-client.go
#CMD ["app"]

ENTRYPOINT [ "app" ]
