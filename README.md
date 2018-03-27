# Silver complete

#[What it is?](#What is it)

#[Current state](#Current state)

#[Warning](#warning)

#[Requirements](#Requirements)

#[Precheks](#Precheks)

### (#What's is it?)

  Check the video, worth a thousand words.

  [![IMAGE ALT TEXT](http://img.youtube.com/vi/ca7T0bXptU8/0.jpg)](http://www.youtube.com/watch?v=ca7T0bXptU8 "Silvercomplete demo")

### Current state

  Proof of Concept

  Tested only on Ubuntu 16.04, but it should work on any other linux, see requirements section


#WARNING

  IT'S INSECURE. Use at your own risk

  All data stored in plain text, connection between client<->server NOT encrypted

  Web ui and etcd api installed without any authentication



### Requirements
  [docker](https://docs.docker.com/install/) (version >= 1.13.0)

  [docker-compose](https://docs.docker.com/compose/install/#install-compose) (version >= 1.11.2)

  X11 compatibility server (like xorg, x11-server)

### Prechecks
```
user $ docker ps | head -1
CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS              PORTS               NAMES
```
If you see error like
```
Got permission denied while trying to connect to the Docker daemon socket at unix:///var/run/docker.sock: Get http://%2Fvar%2Frun%2Fdocker.sock/v1.35/containers/json: dial unix /var/run/docker.sock: connect: permission denied
```

please follow the instructions https://docs.docker.com/install/linux/linux-postinstall/#manage-docker-as-a-non-root-user


### Install in single-user mode

```
git clone https://github.com/metacoma/silvercomplete
cd silvercomplete
docker-compose pull
docker-compose up -d
```

WEB ui for etcd available on http://localhost:2888

ETCD api v2 available on http://localhost:2379


Add example key
```
user@localhost $ curl -XPUT -s 172.16.172.4:2379/v2/keys/myphone -d value=+73084503445
{"action":"set","node":{"key":"/myphone ","value":"+73084503445","modifiedIndex":31,"createdIndex":31}}
```

Also you can manage through key/value through web interface

Open in browser http://localhost:2888


bind hotkey (for example F2) to run next command

```docker exec silvercomplete_gui_1 silvercomplete.sh```

### Installing in multi-user mode
Run these commands on the server, and and remember its ip address (in this example ip addr is 172.16.172.4)
```
user@localhost $ ssh 172.16.172.4
user@172.16.172.4 $ git clone https://github.com/metacoma/silvercomplete
user@172.16.172.4 $ cd silvercomplete
user@172.16.172.4 $ docker-compose pull
user@172.16.172.4 $ docker-compose up -d web etcd
```

WEB ui for etcd available on http://172.16.172.4:2888

ETCD api v2 available on http://172.16.172.4:2379

open in browser: http://172.16.172.4:2888 and add some key

Install on the clients

```
user@localhost $ git clone https://github.com/metacoma/silvercomplete
user@localhost $ cd silvercomplete
user@localhost $ docker-compose pull
user@localhost $ docker-compose run --name silvercomplete_gui_1 -d -e SILVERCOMPLETE_HOST=172.16.172.4 gui
```

Add example key:
```
user@localhost $ curl -XPUT -s 172.16.172.4:2379/v2/keys/myphone -d value=+73084503445
{"action":"set","node":{"key":"/myphoneм-d","value":"+73084503445","modifiedIndex":31,"createdIndex":31}}
```

Also you can manage your key/value through web interface

Open in browser http://172.16.172.4:2888

bind hotkey (for example F2) to run next command

```docker exec -ti silvercomplete_gui_1 silvercomplete.sh```
