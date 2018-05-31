FROM x11docker/lxde
RUN apt update && apt install -y  \
  xdotool                         \
  xterm
