#FROM oldfrostdragon/qt-5.7-xenial-docker:latest
FROM rabits/qt:5.11-desktop
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
  libcpprest-dev                                      \
  libboost-all-dev                                    \
  wget                                                \
  cmake                                               \
  libfuse2                                            \
  m4                                                  \
  rapidjson-dev

RUN apt-get install -y libcurl4-nss-dev
RUN apt-get install -y libcurl4-openssl-dev patchelf
#USER jenkins
RUN git clone https://github.com/Robot/robot
WORKDIR /tmp/robot
RUN git checkout a19be1863405fa4dd5c970946d0f3f59d06b74f1
RUN make -j4 build
USER root
RUN make install
WORKDIR /tmp
RUN git clone https://github.com/metacoma/etcd-cpp-api
WORKDIR /tmp/etcd-cpp-api
ADD https://github.com/catchorg/Catch2/releases/download/v2.2.2/catch.hpp /tmp/etcd-cpp-api/catch.hpp
RUN cmake .
RUN make
RUN make install
WORKDIR /tmp
ENV LD_LIBRARY_PATH=/opt/Qt/5.11.0/gcc_64/lib/
RUN git clone https://github.com/probonopd/linuxdeployqt.git
WORKDIR /tmp/linuxdeployqt
RUN qmake
RUN make -j4 -f Makefile
RUN make install
WORKDIR /tmp
ADD https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/xcb-util-wm/0.4.1-1ubuntu1/xcb-util-wm_0.4.1.orig.tar.gz /tmp/xcb-util-wm_0.4.1.orig.tar.gz
RUN tar zxvf xcb-util-wm_0.4.1.orig.tar.gz
WORKDIR /tmp/xcb-util-wm-0.4.1
RUN ./configure
RUN make
RUN make install
RUN ldconfig -v
WORKDIR /tmp
ADD https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage /usr/local/bin/appimagetool
RUN chmod +x /usr/local/bin/appimagetool
ADD --chown=root:root http://res.freestockphotos.biz/pictures/16/16161-illustration-of-a-silver-key-pv.png /tmp/silverkey-icon.png
RUN chmod a+rw /tmp/silverkey-icon.png
WORKDIR /
#USER jenkins
