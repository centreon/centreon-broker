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
#include "com/centreon/broker/neb/statistics/total_services.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
total_services::total_services() : plugin("total_services") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
total_services::total_services(total_services const& right)
  : plugin(right) {

}

/**
 *  Destructor.
 */
total_services::~total_services() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
total_services& total_services::operator=(total_services const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void total_services::run(
              std::string& output,
	      std::string& perfdata) {
  // Count services.
  unsigned int total(0);
  for (service* s(service_list); s; s = s->next)
    ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " services";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "total_services=" << total;
  perfdata = oss.str();

  return ;
}
