FROM x11docker/xfce
RUN apt-get update
RUN apt-get install -y  \
  libfuse2              \
  xvfb                  \
  xterm                 \
  libfuse2              \
  xdotool               \
  git                   \
  sudo                  \
  x11-utils             \
  x11-apps              \
  fuse

ENV DISPLAY ":99"
RUN echo '#! /bin/sh\n\
Xvfb :99 -pixdepths 24 32 -screen 0 1024x648x24+32\n\
[ -n "$HOME" ] && [ ! -e "$HOME/.config" ] && cp -R /etc/skel/. $HOME/ \n\
exec $*\n\
' > /usr/local/bin/start

RUN useradd -ms /bin/bash -u 1000 user && \
    echo "user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/default

ADD /xfce-config.tgz /home/user/.config/xfce4

RUN chmod +x /usr/local/bin/start
