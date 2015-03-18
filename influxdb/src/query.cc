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
#include "com/centreon/broker/influxdb/query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

std::ostream& operator<<(std::ostream& in, QString const& string) {
  in << string.data();
}

/**
 *  Create an empty query.
 */
query::query()
  : _type(query::unknown) {}

/**
 *  Constructor.
 *
 *  @param[in] naming_scheme  The naming scheme to use.
 */
query::query(std::string const& naming_scheme, data_type type) :
  _naming_scheme_index(0),
  _type(type) {
  _compile_naming_scheme(naming_scheme, type);
}

/**
 *  Copy operator.
 *
 *  @param[in] q  The object to copy.
 */
query::query(query const& q)
  : _compiled_naming_scheme(q._compiled_naming_scheme),
    _compiled_getters(q._compiled_getters),
    _type(q._type) {}

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
    _type = q._type;
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
  if (_type != metric)
    throw (exceptions::msg()
           << "graphite: attempt to generatre metric"
              " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  for (std::vector<void (query::*)(io::data const&, std::ostream&)>::const_iterator
         it(_compiled_getters.begin()),
         end(_compiled_getters.end());
       it != end;
       ++it)
    (this->**it)(me, iss);

  iss << (" ") << me.value << " " << me.ctime << "\n";

  return (iss.str());
}

/**
 *  Generate the query for a status.
 *
 *  @param[in] st  The status.
 *
 *  @return  The query for a status.
 */
std::string query::generate_status(storage::status const& st) {
  if (_type != status)
    throw (exceptions::msg()
           << "graphite: attempt to generatre metric"
              " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  for (std::vector<void (query::*)(io::data const&, std::ostream&)>::const_iterator
         it(_compiled_getters.begin()),
         end(_compiled_getters.end());
       it != end;
       ++it)
    (this->**it)(st, iss);

  iss << (" ") << st.state << " " << st.ctime << "\n";

  return (iss.str());
}

/**
 *  Compile a naming scheme.
 *
 *  @param[in] naming_scheme  The naming scheme to compile.
 *  @param[in] type           The type of this query.
 */
void query::_compile_naming_scheme(
              std::string const& naming_scheme,
              data_type type) {
  size_t found_macro = 0;
  size_t end_macro = 0;

  while ((found_macro = naming_scheme.find_first_of('$', found_macro)) != std::string::npos) {
    std::string substr = naming_scheme.substr(end_macro, found_macro - end_macro);
    if (!substr.empty()) {
      _compiled_naming_scheme.push_back(substr);
      _compiled_getters.push_back(&query::_get_string);
    }

    if ((end_macro = naming_scheme.find_first_of('$', found_macro + 1)) == std::string::npos)
      throw exceptions::msg()
            << "graphite: can't compile query, opened macro not closed: '"
            << naming_scheme.substr(found_macro) << "'";

    std::string macro = naming_scheme.substr(found_macro, end_macro + 1 - found_macro);
    if (macro == "")
      _compiled_getters.push_back(&query::_get_dollar_sign);
    if (macro == "$METRICID$") {
      _throw_on_invalid(metric);
      _compiled_getters.push_back(
        &query::_get_member<unsigned int, storage::metric, &storage::metric::metric_id>);
    }
    else if (macro == "$INSTANCE$")
      _compiled_getters.push_back(
        &query::_get_null);
    else if (macro == "$INSTANCEID$")
      _compiled_getters.push_back(
        &query::_get_member<unsigned int, io::data, &io::data::instance_id>);
    else if (macro == "$HOST$")
      _compiled_getters.push_back(&query::_get_null);
    else if (macro == "$HOSTID$")
      _compiled_getters.push_back(&query::_get_null);
    else if (macro == "$SERVICE$")
      _compiled_getters.push_back(&query::_get_null);
    else if (macro == "$SERVICEID$")
      _compiled_getters.push_back(&query::_get_null);
    else if (macro == "$METRIC$") {
      _throw_on_invalid(metric);
      _compiled_getters.push_back(
        &query::_get_member<QString, storage::metric, &storage::metric::name>);
    }
    else if (macro == "$INDEXID$") {
      _throw_on_invalid(status);
      _compiled_getters.push_back(
        &query::_get_member<unsigned int, storage::status, &storage::status::index_id>);
    }
    else if (macro == "$VALUE$") {
      if (type == metric)
        _compiled_getters.push_back(&query::_get_member<double, storage::metric, &storage::metric::value>);
      else if (type == status)
        _compiled_getters.push_back(&query::_get_member<short, storage::status, &storage::status::state>);
    }
    else if (macro == "$TIME$") {
      if (type == metric)
        _compiled_getters.push_back(&query::_get_member<timestamp, storage::metric, &storage::metric::ctime>);
      else if (type == status)
        _compiled_getters.push_back(&query::_get_member<timestamp, storage::status, &storage::status::ctime>);
    }
    else
      logging::config(logging::high)
        << "graphite: unknown macro '" << macro << "': ignoring it";
    found_macro = end_macro = end_macro + 1;
  }
  std::string substr = naming_scheme.substr(end_macro, found_macro - end_macro);
  if (!substr.empty()) {
    _compiled_naming_scheme.push_back(substr);
    _compiled_getters.push_back(&query::_get_string);
  }
}

/**
 *  Throw on invalid macro type.
 *
 *  @param[in] macro_type  The macro type;
 */
void query::_throw_on_invalid(data_type macro_type) {
  if (macro_type != _type)
    throw (exceptions::msg()
           << "graphite: macro of invalid type");
}

/*
 * Getters
 */

/**
 *  Get a string in the compiled naming scheme.
 *
 *  @param[in] d     The data, unused.
 *  @param[out] is   The stream.
 */
void query::_get_string(io::data const& d, std::ostream& is) {
  (void)d;
  is << _compiled_naming_scheme[_naming_scheme_index++];
}

/**
 *  Null getter.
 *
 *  @param[in] d    The data, unused.
 *  @param[out] is  The stream, unused;
 */
void query::_get_null(io::data const& d, std::ostream& is) {
  (void)d;
  (void)is;
}

/**
 *  Get a dollar sign (for escape).
 *
 *  @param[in] d  unused.
 *  @param[in]    The stream.
 */
void query::_get_dollar_sign(io::data const& d, std::ostream& is) {
  is << "$";
}
