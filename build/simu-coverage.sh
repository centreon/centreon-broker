#!/bin/bash

CENTREON_BROKER="$HOME/centreon-broker"

lcov --directory . --zerocounters
/usr/sbin/cbd /etc/centreon-broker/central-broker.xml
lcov --directory . --capture --output-file simu-coverage.info
lcov --remove simu-coverage.info "$CENTREON_BROKER/simu/*" "$CENTREON_BROKER/build/*" "/usr/include/*" '*/test/*' --output-file simu-coverage.info.cleaned
genhtml -o simu-coverage simu-coverage.info.cleaned
rm simu-coverage.info simu-coverage.info.cleaned
