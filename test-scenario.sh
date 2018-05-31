#!/bin/sh
set -ex
TMP_FILE="/tmp/`date +%s`"
KEY="foo"
VALUE="bar"

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

WINDOW_NAME="silverkey-xterm-test"
xterm -xrm "xterm*allowTitleOps: false" -title "${WINDOW_NAME}" -n "${WINDOW_NAME}" &
xterm_filter="--all --pid $! --name ${WINDOW_NAME}"
xdotool_wait 10 ${xterm_filter}
xdotool search ${xterm_filter} windowactivate --sync %1 type 'export SILVERKEY_VALUE='
~/bin/silverkey >/dev/null 2>&1 &
silverkey_filter="--all --pid $! --class silverkey"
xdotool_wait 20 ${silverkey_filter}
xdotool search ${silverkey_filter} windowactivate --sync %1 type "/${KEY}"
xdotool search ${silverkey_filter} key KP_Enter
sleep 5
xdotool search ${xterm_filter} windowactivate --sync %1 type "$(printf '\n ')"
xdotool search ${xterm_filter} type "echo -n \$SILVERKEY_VALUE > $TMP_FILE"
xdotool search ${xterm_filter} windowactivate --sync %1 type "$(printf '\n ')"
if [ "`cat $TMP_FILE`" = "$VALUE" ]; then
  echo "TEST OK"
  exit 0
else
  echo "TEST FAIL"
  exit 1
fi
