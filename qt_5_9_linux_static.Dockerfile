FROM ubuntu:xenial

RUN apt-get update && apt-get install -y \
  build-essential                        \
  perl                                   \
  python                                 \
  "^libxcb.*"                            \
  libx11-xcb-dev                         \
  libglu1-mesa-dev                       \
  libxrender-dev                         \
  libedit-dev                            \
  flex                                   \
  bison                                  \
  gperf                                  \
  libicu-dev                             \
  libxslt-dev                            \
  git

WORKDIR /build
RUN git clone git://code.qt.io/qt/qt5.git qt5
WORKDIR /build/qt5
RUN perl init-repository                        \
    --module-subset=all,-qt3d,-qtandroidextras,-qtcanvas3d,-qtdatavis3d,-qtdoc,-qtgamepad,-qtmultimedia,-qtserialport,-qtspeech,-qtvirtualkeyboard,-qtwebview,-qtwebglplugin,-qtwebengine,-qtwayland


ENV PATH="/build/qt5/qtbase/bin:/build/qt5/qtrepotools/bin:$PATH"
RUN ./configure                                 \
  -release                                      \
  -nomake tools                                 \
  -nomake examples                              \
  -nomake tests                                 \
  -prefix /qt5                                  \
  -confirm-license                              \
  -opensource                                   \
  -feature-xml-schema                           \
  -static

RUN make -j4
RUN make -j4 install
# XXX move to upper section
RUN apt-get update && apt install -y rapidjson-dev libx11-dev libxtst-dev libxinerama-dev libcurl4-nss-dev
WORKDIR /tmp
RUN git clone https://github.com/Robot/robot
WORKDIR /tmp/robot
RUN git checkout a19be1863405fa4dd5c970946d0f3f59d06b74f1
RUN make -j4 build
USER root
RUN make install
RUN jenkins
