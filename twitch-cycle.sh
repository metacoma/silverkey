#!/bin/sh
while :; do
  docker run --rm -it --user user:user --network=silverkey-ci-test --privileged --volumes-from=silverkeyci_ci_1 -v `pwd`:/opt/silverkey --entrypoint /bin/sh silverkey-ci-wm-xfce:latest -c 'startxfce4 & sleep 10; /opt/silverkey/test-scenario.sh'
  echo "XFCE - TEST: $?"
  docker run --rm -it --user user:user --network=silverkey-ci-test --privileged --volumes-from=silverkeyci_ci_1 -v `pwd`:/opt/silverkey --entrypoint /bin/sh silverkey-ci-wm-fvwm:latest -c 'fvwm -replace & sleep 10; /opt/silverkey/test-scenario.sh'
  echo "FVWM - TEST: $?"
done
