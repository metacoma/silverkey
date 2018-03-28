#!/bin/sh

ETCD_HOST=http://etcd:2379
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
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/масса -d value=$mass
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/кипение -d value=$boil
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/плавление -d value=$melt
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/период -d value=$period
  curl -XPUT -s ${ETCD_HOST}/v2/keys/chemistry/$element/номер -d value=$number
done


