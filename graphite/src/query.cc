/*
** Copyright 2011-2014 Merethis
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

#include <algorithm>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/graphite//query.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

template <typename T>
/**
 *  Transform a value to string.
 *
 *  @param[in] val  The value.
 *
 *  @return         A string containing the value.
 */
std::string to_string(T val) {
  std::stringstream ss;
  ss << val;
  return (ss.str());
}

/**
 *  Constructor.
 *
 *  @param[in] naming_scheme  The naming scheme to use.
 */
query::query(std::string const& naming_scheme) {
  _naming_scheme = naming_scheme;
}

/**
 *  Copy operator.
 *
 *  @param[in] q  The object to copy.
 */
query::query(query const& q)
  : _naming_scheme(q._naming_scheme) {}

/**
 *  Destructor
 */
query::~query() {}

/**
 *  Assignment operator.
 *
 *  @param[in] q  The object to copy.
 *
 *  @return       A reference to this object.
 */
query& query::operator=(query const& q) {
  if (this != &q) {
    _naming_scheme = q._naming_scheme;
  }
  return (*this);
}

/**
 *  Generate the query for a metric.
 *
 *  @param[in] me  The metric.
 *
 *  @return  The query for a metric.
 */
std::string query::generate_metric(storage::metric const& me) {
  std::string tmp;

  return (tmp);
}

/**
 *  Generate the query for a status.
 *
 *  @param[in] st  The status.
 *
 *  @return  The query for a status.
 */
std::string query::generate_status(storage::status const& st) {

}

/**
 *  Replace in one pass.
 *
 *  @param[out] ret     The string returned.
 *  @param[in]  macros  A pair of key->value macros.
 */
void query::_one_pass_replace(
              std::string& ret,
              std::map<std::string, std::string> const& macros) {
}
