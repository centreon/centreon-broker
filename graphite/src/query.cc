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
#include "com/centreon/broker/graphite/query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

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
query::query(std::string const& naming_scheme) :
  _naming_scheme_index(0) {
  _compile_naming_scheme(naming_scheme);
}

/**
 *  Copy operator.
 *
 *  @param[in] q  The object to copy.
 */
query::query(query const& q)
  : _compiled_naming_scheme(q._compiled_naming_scheme),
    _compiled_getters(q._compiled_getters) {}

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
    _compiled_naming_scheme = q._compiled_naming_scheme;
    _compiled_getters = q._compiled_getters;
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
  _naming_scheme_index = 0;
  _type = metric;
  std::string tmp;
  tmp.reserve(64);
  for (std::vector<std::string (query::*)(io::data const&)>::const_iterator
         it(_compiled_getters.begin()),
         end(_compiled_getters.end());
       it != end;
       ++it)
    tmp.append((this->**it)(me));

  tmp.append(to_string(me.value));
  tmp.append(" ");
  tmp.append(to_string(me.ctime));
  tmp.append("\n");

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
  _naming_scheme_index = 0;
  _type = status;
  std::string tmp;
  tmp.reserve(64);
  for (std::vector<std::string (query::*)(io::data const&)>::const_iterator
         it(_compiled_getters.begin()),
         end(_compiled_getters.end());
       it != end;
       ++it)
    tmp.append((this->**it)(st));

  tmp.append(to_string(st.state));
  tmp.append(" ");
  tmp.append(to_string(st.ctime));
  tmp.append("\n");

  return (tmp);
}

/**
 *  Compile a naming scheme.
 *
 *  @param[in] naming_scheme  The naming scheme to compile.
 */
void query::_compile_naming_scheme(std::string const& naming_scheme) {
  size_t found_macro = 0;
  size_t end_macro = 0;

  while ((found_macro = naming_scheme.find_first_of('$', found_macro)) != std::string::npos) {
    std::string substr = naming_scheme.substr(end_macro, found_macro);
    if (!substr.empty()) {
      _compiled_naming_scheme.push_back(substr);
      _compiled_getters.push_back(&query::_get_string);
    }

    if ((end_macro = naming_scheme.find_first_of('$', found_macro + 1)) == std::string::npos)
      throw exceptions::msg()
            << "graphite: can't compile query, opened macro not closed: '"
            << naming_scheme.substr(found_macro) << "'";

    std::string macro = naming_scheme.substr(found_macro, end_macro + 1);
    if (macro == "$METRICID$")
      _compiled_getters.push_back(&query::_get_metric);
    else if (macro == "$INSTANCE$")
      _compiled_getters.push_back(&query::_get_instance);
    else if (macro == "$INSTANCEID$")
      _compiled_getters.push_back(&query::_get_instance_id);
    else if (macro == "$HOST$")
      _compiled_getters.push_back(&query::_get_host);
    else if (macro == "$HOSTID$")
      _compiled_getters.push_back(&query::_get_host_id);
    else if (macro == "$SERVICE$")
      _compiled_getters.push_back(&query::_get_service);
    else if (macro == "$SERVICEID$")
      _compiled_getters.push_back(&query::_get_service_id);
    else if (macro == "$METRIC$")
      _compiled_getters.push_back(&query::_get_metric);
    else if (macro == "$METRICID$")
      _compiled_getters.push_back(&query::_get_metric_id);
    else if (macro == "$INDEXID$")
      _compiled_getters.push_back(&query::_get_index_id);
    else
      logging::config(logging::high)
        << "graphite: unknown macro '" << macro << "': ignoring it";
    found_macro = end_macro + 1;
  }
  std::string substr = naming_scheme.substr(end_macro, found_macro);
  if (!substr.empty()) {
    _compiled_naming_scheme.push_back(substr);
    _compiled_getters.push_back(&query::_get_string);
  }
}

/*
 * Getters
 */

/**
 *  Get a string in the compiled naming scheme.
 *
 *  @param[in] d  The data, unused.
 *
 *  @return       The string.
 */
std::string query::_get_string(io::data const& d) {
  return (_compiled_naming_scheme[_naming_scheme_index++]);
}

/**
 *  Get a metric id from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_metric_id(io::data const& d) {
  if (_type == metric)
    return (to_string(static_cast<storage::metric const&>(d).metric_id));
  else
    throw exceptions::msg()
          << "graphite: can't get metric id of a non metric query";
}

/**
 *  Get a metric from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_metric(io::data const& d) {
  return ("");
}

/**
 *  Get a index id from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_index_id(io::data const& d) {
  if (_type == status)
    return (to_string(static_cast<storage::status const&>(d).index_id));
  throw exceptions::msg()
        << "graphite: can't get metric id of a non status query";
}

/**
 *  Get an instance id from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_instance_id(io::data const& d) {
  return (to_string(d.instance_id));
}

/**
 *  Get an instance from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_instance(io::data const& d) {
  return ("");
}

/**
 *  Get a host id from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_host_id(io::data const& d) {
  return ("");
}

/**
 *  Get a host from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_host(io::data const& d) {
  return ("");
}

/**
 *  Get a service id from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_service_id(io::data const& d) {
  return ("");
}

/**
 *  Get a service from the data.
 *
 *  @param[in] d  The data.
 *
 *  @return       The string.
 */
std::string query::_get_service(io::data const& d) {
  return ("");
}
