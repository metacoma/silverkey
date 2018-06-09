#!/bin/sh
#set -ex
TMP_FILE="/tmp/`date +%s`"
KEY="chemistry/gold/boil"
VALUE="3243"

jenkins_latest_build() {
  ls -ltr /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/ | awk '/^d.*[0-9]+?$/ {build=$NF} END {print build}'
}

jenkins_build_dir() {
  echo /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/$1
}

jenkins_silverkey_path() {
  echo `jenkins_build_dir $1`/archive/Silverkey-x86_64.AppImage
}

jenkins_silverkey_commit() {
  # xxx fixme use xmldoc
  cat `jenkins_build_dir $1`/build.xml | sed -nr '/<commit>/ {s,(</?commit>| ),,g;p}'
}

xmessage_notify() {
	xmessage -timeout 5 -center -file -
}


latest_build_number=`jenkins_latest_build`
latest_commit=`jenkins_silverkey_commit ${latest_build_number}`
#SILVERKEY_BIN=${1:-/tmp/silverkey}
#SILVERKEY_BIN=~/bin/silverkey
SILVERKEY_BIN=`jenkins_silverkey_path ${latest_build_number}`


echo "Test build: ${latest_build_number} commit $latest_commit" | xmessage_notify

xdotool_wait() {
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

irssi_notify() {
	:
	#printf "msg #silverkey_project $*\n\n" >> ~/.irssi/remote-control
}

WINDOW_NAME="some-roxterm"
xterm -fa monaco -fs 23 -xrm "xterm*allowTitleOps: false" -g 200x40+20+30 -title "${WINDOW_NAME}" &
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
xdotool search ${silverkey_filter} windowactivate --sync %1 type --delay 180 "/${KEY}"
xdotool mousemove 250 200
xdotool search ${term_filter} windowactivate
xdotool search ${silverkey_filter} key KP_Enter

sleep 2
xdotool search ${term_filter} windowactivate --sync %1 type "$(printf '\n ')"
xdotool search ${term_filter} type "echo -n \$SILVERKEY_VALUE > $TMP_FILE"
xdotool search ${term_filter} windowactivate --sync %1 type "$(printf '\n ')"
sleep 5
kill -9 ${xterm_pid}
if [ "`cat $TMP_FILE`" = "$VALUE" ]; then
  echo "TEST OK"
  echo "TEST OK" | xmessage_notify
  irssi_notify "$$ TEST OK"
  exit 0
else
  echo "TEST FAIL"
  echo "TEST FAIL" | xmessage_notify
  irssi_notify "$$ TEST FAIL"
  exit 1
fi
