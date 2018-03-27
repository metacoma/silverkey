#!/bin/sh

lookup_key() {
  # | jq -r '..|select(.key == "'"$1"'")?|.value' 2>/dev/null| grep -v null | head -n1
  curl -sL http://$SILVERCOMPLETE_HOST/v2/keys$1 | jq -r '.node.value'

}

insert_value() {
  sed -r 's/(.)/\1 /g' | sed 's/\./period/g'
}


key="$1"
no_del="$2"
echo "ARG KEY = '$key'" >> /tmp/log
value=`lookup_key $key`
if [ -n "${value}" ]; then
  echo "KEY '$key' = '$value'" >> /tmp/log
  test -z "${no_del}" && xdotool key `echo "$key " | sed 's/./BackSpace /g'`
  xdotool type "$value"
fi

exit 0
