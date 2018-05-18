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
RUN perl init-repository

ENV PATH="/build/qt5/qtbase/bin:/build/qt5/qtrepotools/bin:$PATH"
RUN ./configure                                 \
  -nomake examples                              \
  -nomake tests                                 \
  -prefix /qt5                                  \
  -confirm-license                              \
  -opensource                                   \
  -static

RUN ./build -j4
RUN make -j4


