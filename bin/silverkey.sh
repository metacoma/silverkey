#!/bin/sh

#key=`curl -s 213.59.223.213:2379/v2/keys/?recursive=true | jq -r '..|.key?' | grep -v '^null$' | rofi -dmenu -i`
#key=`cat ~/spaces/thaw/data/* | jq -r '..|.key?' | grep -v '^null$' | rofi -dmenu -i`
key=`curl -s ${SILVERCOMPLETE_HOST}/v2/keys/?recursive=true | jq -r '..|.key?' | grep -v '^null$' | rofi -dmenu -mesg Keys -markup -i`
silvercomplete-lookup.sh $key nodel
