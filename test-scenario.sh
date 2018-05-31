#!/bin/sh
#set -ex
TMP_FILE="/tmp/`date +%s`"
KEY="chemistry/gold/boil"
VALUE="3243"

SILVERKEY_BIN=/tmp/silverkey
#SILVERKEY_BIN=~/bin/silverkey

xdotool_wait() {
  #local MAX_RETRIES=60
  local tries=$1
  shift
  local xdotool_filter="$*"
  for i in `seq 1 ${tries}`; do
    echo -n .
    xdotool search $xdotool_filter && return 0 || :
    sleep 1
  done
  return 1
}

WINDOW_NAME="some-roxterm"
xterm -xrm "xterm*allowTitleOps: false" -g 200x40+20+30 -title "${WINDOW_NAME}" &
#roxterm --geometry=100x30+20+30 -T "${WINDOW_NAME}" &
xterm_pid=$!
term_filter="--all --pid ${xterm_pid} --name ${WINDOW_NAME}"
xdotool_wait 10 ${term_filter}
xdotool mousemove 250 200
xdotool search ${term_filter} windowactivate
xdotool search ${term_filter} windowactivate --sync %1 type 'export SILVERKEY_VALUE='
${SILVERKEY_BIN} >/dev/null 2>&1 &
silverkey_filter="--all --pid $! --class silverkey"
xdotool_wait 20 ${silverkey_filter}
xdotool search ${silverkey_filter} windowactivate --sync %1 type --delay 100 "/${KEY}"
xdotool mousemove 250 200
xdotool search ${term_filter} windowactivate
xdotool search ${silverkey_filter} key KP_Enter

xdotool search ${term_filter} windowactivate --sync %1 type "$(printf '\n ')"
xdotool search ${term_filter} type "echo -n \$SILVERKEY_VALUE > $TMP_FILE"
xdotool search ${term_filter} windowactivate --sync %1 type "$(printf '\n ')"
kill -9 ${xterm_pid}
if [ "`cat $TMP_FILE`" = "$VALUE" ]; then
  echo "TEST OK"
  exit 0
else
  echo "TEST FAIL"
  exit 1
fi


