#!/bin/bash

export BUNDLE_PATH=${BUNDLE_PATH:-/Applications/silverkey-qt.app}
TEST_DIR=$(dirname ${BASH_SOURCE[0]})
BASE_LIB_FILE="${TEST_DIR}/SKBase.applescript"

BASE_LIB=$(cat ${BASE_LIB_FILE})

ERR_COUNT=0


function runAll() {
    for tst_file in $(ls ${TEST_DIR}/test_*)
    do
        tst=$(cat ${TEST_DIR}/${tst_file})
        echo  -e "${BASE_LIB}\n${tst}" | osascript -l JavaScript
        if [ $? != 0 ]
        then
            let ERR_COUNT=${ERR_COUNT} + 1
        fi
        echo
    done
}

function runSingleTest()
{
    test_name=$1
    tst=$(cat ${TEST_DIR}/${test_name}.applescript)
    echo  -e "${BASE_LIB}\n${tst}" | osascript -l JavaScript
    if [ $? != 0 ]
    then
        let ERR_COUNT=${ERR_COUNT}+1
    fi
}

if [ "$1" != "" ]
then
    runSingleTest $1
else
    runAll
fi

if [ $ERR_COUNT != 0 ]
then
    echo "ERROR: ${ERR_COUNT} tests failed"
    exit 1
else
    echo "SUCCESS: All tests passed"
fi
