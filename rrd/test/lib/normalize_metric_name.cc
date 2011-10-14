/*
** Copyright 2011 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <string.h>
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon::broker;

/**
 *  Check metric name normalization.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // #1.
  {
    QString normalized(rrd::lib::normalize_metric_name(
      "mymetric01234"));
    retval |= (normalized.size() > rrd::lib::max_metric_length);
    retval |= strncmp(
      qPrintable(normalized),
      "mymetric01234",
      rrd::lib::max_metric_length);
  }

  // #2.
  {
    QString normalized(rrd::lib::normalize_metric_name(
      "C:\\ Used Space"));
    retval |= (normalized.size() > rrd::lib::max_metric_length);
    retval |= strncmp(
      qPrintable(normalized),
      "C-_bslash-Used-Space",
      rrd::lib::max_metric_length);
  }

  return (retval);
}
