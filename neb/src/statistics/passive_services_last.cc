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
#include "com/centreon/broker/neb/statistics/passive_services_last.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
passive_services_last::passive_services_last()
  : plugin("passive_services_last") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
passive_services_last::passive_services_last(passive_services_last const& right)
 : plugin(right) {

}

/**
 *  Destructor.
 */
passive_services_last::~passive_services_last() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
passive_services_last& passive_services_last::operator=(passive_services_last const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void passive_services_last::run(
       std::string& output,
       std::string& perfdata) {
  unsigned int last_checked_1(0);
  unsigned int last_checked_5(0);
  unsigned int last_checked_15(0);
  unsigned int last_checked_60(0);
  time_t now(time(NULL));
  for (service* s(service_list); s; s = s->next) {
    if (s->check_type == SERVICE_CHECK_PASSIVE) {
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

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString() << " received "
      << last_checked_5 << " passive checks during the last 5 minutes";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "passive_services_last_1=" << last_checked_1
      << " passive_services_last_5=" << last_checked_5
      << " passive_services_last_15=" << last_checked_15
      << " passive_services_last_60=" << last_checked_60;
  perfdata = oss.str();

  return ;
}
