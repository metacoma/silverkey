FROM ubuntu:bionic

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y                      \
      xvfb                                            \
      ffmpeg                                          \
      irssi                                           \
      tmux                                            \
      libfuse2                                        \
      xdotool                                         \
      sudo                                            \
      xterm                                           \
      fuse                                            \
      x11-apps

RUN apt-get install -y dbus-x11 procps psmisc x11-utils x11-xserver-utils xosd-bin

# Xfce
RUN apt-get install -y --no-install-recommends xfce4
RUN apt-get install -y gtk3-engines-xfce xfce4-notifyd

# additional Xfce goodies
RUN apt-get install -y mousepad xfce4-taskmanager xfce4-terminal

# includes GTK3 broadway support for HTML5 web applications
RUN apt-get install -y --no-install-recommends libgtk-3-bin

# additional Xfce panel plugins
RUN apt-get install -y --no-install-recommends xfce4-battery-plugin \
    xfce4-clipman-plugin xfce4-cpugraph-plugin xfce4-datetime-plugin \
    xfce4-netload-plugin xfce4-notes-plugin xfce4-places-plugin \
    xfce4-sensors-plugin xfce4-systemload-plugin \
    xfce4-whiskermenu-plugin xfce4-indicator-plugin \
    xfce4-cpufreq-plugin xfce4-diskperf-plugin xfce4-fsguard-plugin \
    xfce4-genmon-plugin xfce4-smartbookmark-plugin xfce4-timer-plugin \
    xfce4-verve-plugin xfce4-weather-plugin


ENV DISPLAY "xvfb:99"
RUN echo '#! /bin/sh\n\
sudo -i -u user startxfce4\n\
' > /usr/local/bin/start
RUN chmod +x /usr/local/bin/start

#xterm -g 600x50+0+350 -T chat -e sh -c 'tmux attach -t irssi'
RUN useradd -ms /bin/bash -u 1000 user && \
    echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/default

ADD /xfce-config.tgz /home/user/.config/xfce4
ENV SILVERKEY_CONFIG "/home/user/.config/Unknown Organization/silverkey.conf"
COPY ./silverkey.conf $SILVERKEY_CONFIG

RUN chown -R user:user /home/user


ENTRYPOINT [ "/usr/local/bin/start" ]

