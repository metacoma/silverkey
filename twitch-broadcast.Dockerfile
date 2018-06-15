FROM ubuntu:bionic

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y                      \
      ffmpeg                                          \
      x11-apps

RUN apt-get install -y dbus-x11 procps psmisc x11-utils x11-xserver-utils xosd-bin

ENV DISPLAY "xvfb:99"

#xterm -g 600x50+0+350 -T chat -e sh -c 'tmux attach -t irssi'
#RUN useradd -ms /bin/bash -u 1000 user && \
#    echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/default


ADD ./twitch_ci_broadcast.sh /usr/local/bin/twitch_ci_broadcast.sh
ADD ./rhapsody_in_blue.mp3 /tmp/rhapsody_in_blue.mp3
WORKDIR /tmp
ENTRYPOINT ["/usr/local/bin/twitch_ci_broadcast.sh"]
