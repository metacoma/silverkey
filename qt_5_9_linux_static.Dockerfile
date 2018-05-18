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
RUN perl init-repository                                        \
    --module-subset=all,-qt3d,-qtandroidextras,-qtcanvas3d,-qtdatavis3d,-qtdoc,-qtgamepad,-qtmultimedia,-qtserialport,-qtspeech,-qtvirtualkeyboard,-qtwebview,-qtwebglplugin,-qtwebengine,-qtwayland  \
    --mirror git://github.com/qt/


ENV PATH="/build/qt5/qtbase/bin:/build/qt5/qtrepotools/bin:$PATH"
RUN ./configure                                 \
  -release                                      \
  -nomake tools                                 \
  -nomake examples                              \
  -nomake tests                                 \
  -no-webkit                                    \
  -no-qt3support                                \
  -no-script                                    \
  -no-scripttools                               \
  -no-qt3d                                      \
  -prefix /qt5                                  \
  -confirm-license                              \
  -opensource                                   \
  -feature-xml-schema                           \
  -static

RUN ./build -j4
RUN make -j4
