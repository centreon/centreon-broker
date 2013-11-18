/*
** Copyright 2013 Merethis
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

#include <sstream>
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/active_services_last.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
active_services_last::active_services_last() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
active_services_last::active_services_last(active_services_last const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
active_services_last::~active_services_last() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
active_services_last& active_services_last::operator=(active_services_last const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string active_services_last::run() {
  unsigned int last_checked_1(0);
  unsigned int last_checked_5(0);
  unsigned int last_checked_15(0);
  unsigned int last_checked_60(0);
  time_t now(time(NULL));
  for (service* s(service_list); s; s = s->next) {
    if (s->check_type == SERVICE_CHECK_ACTIVE) {
      int diff(now - s->last_check);
      if (diff <= 60 * 60) {
        ++last_checked_60;
        if (diff <= 15 * 60) {
          ++last_checked_15;
          if (diff <= 5 * 60) {
            ++last_checked_5;
            if (diff <= 1 * 60)
              ++last_checked_1;
          }
        }
      }
    }
  }
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString() << " ran "
      << last_checked_5 << " active checks during the last 5 minutes|"
      << "active_services_last_1=" << last_checked_1
      << " active_services_last_5=" << last_checked_5
      << " active_services_last_15=" << last_checked_15
      << " active_services_last_60=" << last_checked_60 << "\n";
  return (oss.str());
}
