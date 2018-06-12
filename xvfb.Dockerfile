# x11docker/lxde
#
# Run LXDE desktop in docker.
# Use x11docker to run image.
# Get x11docker from github:
#   https://github.com/mviereck/x11docker
#
# Examples:
#  - Run desktop:
#      x11docker --desktop x11docker/lxde
#  - Run single application:
#      x11docker x11docker/lxde pcmanfm
#
# Options:
# Persistent home folder stored on host with   --home
# Shared host folder with                      --sharedir DIR
# Hardware acceleration with option            --gpu
# Clipboard sharing with option                --clipboard
# Sound support with option                    --alsa
# With pulseaudio in image, sound support with --pulseaudio
#
# See x11docker --help for further options.

FROM debian:stretch
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get  update
RUN apt-get install -y dbus-x11 procps psmisc xvfb xterm libfuse2 xdotool git sudo fuse

# OpenGL / MESA
RUN apt-get install -y mesa-utils mesa-utils-extra libxv1 kmod xz-utils

# Language/locale settings
# replace en_US by your desired locale setting,
# for example de_DE for german.
ENV LANG en_US.UTF-8
RUN echo $LANG UTF-8 > /etc/locale.gen
RUN apt-get install -y locales && update-locale --reset LANG=$LANG

# some utils to have proper menus, mime file types etc.
RUN apt-get install -y --no-install-recommends xdg-utils xdg-user-dirs \
    menu menu-xdg mime-support desktop-file-utils

ENV DISPLAY ":99"
RUN echo '#! /bin/sh\n\
Xvfb ${DISPLAY} -pixdepths 24 32 -listen tcp -ac -screen 0 1920x1080x24\n\
' > /usr/local/bin/start
RUN chmod +x /usr/local/bin/start

ENTRYPOINT ["/usr/local/bin/start"]
