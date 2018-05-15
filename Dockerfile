FROM oldfrostdragon/qt-5.7-xenial-docker:latest
WORKDIR /tmp/
USER root
RUN apt-get purge -y libcurl3 libcurl3-gnutls
RUN apt-get update && apt-get install -y --upgrade    \
  git build-essential                                 \
  libxtst-dev                                         \
  libx11-dev                                          \
  libxinerama-dev                                     \
  libcurlpp-dev                                       \
  libcurl4-openssl-dev                                \
  libegl1-mesa                                        \
  rapidjson-dev
RUN apt-get install -y libcurl4-nss-dev
RUN apt-get install -y libcurl4-openssl-dev patchelf
USER jenkins
RUN git clone https://github.com/Robot/robot
WORKDIR /tmp/robot
RUN git checkout a19be1863405fa4dd5c970946d0f3f59d06b74f1
RUN make -j4 build
USER root
RUN make install
WORKDIR /tmp
RUN git clone https://github.com/probonopd/linuxdeployqt.git
WORKDIR /tmp/linuxdeployqt
RUN qmake
RUN make -j4 -f Makefile
RUN make install
USER jenkins
