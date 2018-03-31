#!/bin/sh

ETCD_HOST=http://localhost:2379
TEMP_FILE=/tmp/chemistry


show_chemistry() {
  test -f ${TEMP_FILE} || curl -s -o ${TEMP_FILE} https://raw.githubusercontent.com/Bowserinator/Periodic-Table-JSON/master/PeriodicTableJSON.json
  cat ${TEMP_FILE}
}

elements=`show_chemistry | jq -r '.elements[].name'`

for element in $elements; do
  #show_chemistry | jq '[.elements[]|select(.name == "'$elements'")'
  mass=`show_chemistry | jq -r '.elements[]|select(.name == "'$element'").atomic_mass'`
  boil=`show_chemistry | jq -r '.elements[]|select(.name == "'$element'").boil'`
  melt=`show_chemistry | jq -r '.elements[]|select(.name == "'$element'").melt'`
  period=`show_chemistry | jq -r '.elements[]|select(.name == "'$element'").period'`
  number=`show_chemistry | jq -r '.elements[]|select(.name == "'$element'").number'`
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/mass -d value=$mass
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/boil -d value=$boil
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/melt -d value=$melt
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/period -d value=$period
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/number -d value=$number
done


