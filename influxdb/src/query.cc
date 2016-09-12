/*
** Copyright 2011-2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <algorithm>
#include <sstream>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

std::ostream& operator<<(std::ostream& in, QString const& string) {
  in << string.toStdString();
  return (in);
}

/**
 *  Create an empty query.
 */
query::query()
  : _type(query::unknown),
    _cache(NULL) {}

/**
 *  Constructor.
 *
 *  @param[in] naming_scheme  The naming scheme to use.
 */
query::query(
        std::string const& naming_scheme,
        data_type type,
        macro_cache const& cache,
        bool escape) :
  _naming_scheme_index(0),
  _type(type),
  _cache(&cache),
  _escape(escape) {
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
    _type(q._type),
    _cache(q._cache) {}

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
    _cache = q._cache;
  }
  return (*this);
}

/**
 *  Escape string for influxdb10
 *
 *  @param str  The string.
 *  @return     The string, escaped.
 */
static std::string escape(std::string const& str) {
  std::string ret(str);
  ::com::centreon::broker::misc::string::replace(ret, " ", "\\ ");
  ::com::centreon::broker::misc::string::replace(ret, ",", "\\,");
  return (ret);
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
           << "influxdb: attempt to generate metric"
              " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  try {
    for (std::vector<void (query::*)(io::data const&, std::ostream&)>::const_iterator
           it(_compiled_getters.begin()),
           end(_compiled_getters.end());
         it != end;
         ++it) {
      if (!_escape || *it == &query::_get_string)
        (this->**it)(me, iss);
      else {
        std::ostringstream escaped;
        (this->**it)(me, escaped);
        iss << escape(escaped.str());
      }
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "influxdb: couldn't generate query for metric "
      << me.metric_id << ": " << e.what();
    return ("");
  }

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
           << "influxdb: attempt to generate status"
              " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  try {
    for (std::vector<void (query::*)(io::data const&, std::ostream&)>::const_iterator
           it(_compiled_getters.begin()),
           end(_compiled_getters.end());
         it != end;
         ++it) {
      if (!_escape || *it == &query::_get_string)
        (this->**it)(st, iss);
      else {
        std::ostringstream escaped;
        (this->**it)(st, escaped);
        iss << escape(escaped.str());
      }
    }
  } catch (std::exception const& e) {
    logging::error(logging::medium)
      << "influxdb: couldn't generate query for status "
      << st.index_id << ": " << e.what();
    return ("");
  }

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

  while ((found_macro = naming_scheme.find_first_of('$', found_macro))
           != std::string::npos) {
    std::string substr = naming_scheme.substr(
                           end_macro,
                           found_macro - end_macro);
    if (!substr.empty()) {
      _compiled_naming_scheme.push_back(substr);
      _compiled_getters.push_back(&query::_get_string);
    }

    if ((end_macro = naming_scheme.find_first_of('$', found_macro + 1))
          == std::string::npos)
      throw exceptions::msg()
            << "graphite: can't compile query, opened macro not closed: '"
            << naming_scheme.substr(found_macro) << "'";

    std::string macro = naming_scheme.substr(
                          found_macro,
                          end_macro + 1 - found_macro);
    if (macro == "")
      _compiled_getters.push_back(&query::_get_dollar_sign);
    if (macro == "$METRICID$") {
      _throw_on_invalid(metric);
      _compiled_getters.push_back(
        &query::_get_member<
                  unsigned int,
                  storage::metric,
                  &storage::metric::metric_id>);
    }
    else if (macro == "$INSTANCE$")
      _compiled_getters.push_back(
        &query::_get_instance);
    else if (macro == "$INSTANCEID$")
      _compiled_getters.push_back(
        &query::_get_member<unsigned int, io::data, &io::data::source_id>);
    else if (macro == "$HOST$")
      _compiled_getters.push_back(&query::_get_host);
    else if (macro == "$HOSTID$")
      _compiled_getters.push_back(&query::_get_host_id);
    else if (macro == "$SERVICE$")
      _compiled_getters.push_back(&query::_get_service);
    else if (macro == "$SERVICEID$")
      _compiled_getters.push_back(&query::_get_service_id);
    else if (macro == "$METRIC$") {
      _throw_on_invalid(metric);
      _compiled_getters.push_back(
        &query::_get_member<QString, storage::metric, &storage::metric::name>);
    }
    else if (macro == "$INDEXID$") {
      _compiled_getters.push_back(
        &query::_get_index_id);
    }
    else if (macro == "$VALUE$") {
      if (type == metric)
        _compiled_getters.push_back(
          &query::_get_member<
                    double,
                    storage::metric,
                    &storage::metric::value>);
      else if (type == status)
        _compiled_getters.push_back(
          &query::_get_member<
                    short,
                    storage::status,
                    &storage::status::state>);
    }
    else if (macro == "$TIME$") {
      if (type == metric)
        _compiled_getters.push_back(
          &query::_get_member<
                    timestamp,
                    storage::metric,
                    &storage::metric::ctime>);
      else if (type == status)
        _compiled_getters.push_back(
          &query::_get_member<
                    timestamp,
                    storage::status,
                    &storage::status::ctime>);
    }
    else
      logging::config(logging::high)
        << "graphite: unknown macro '" << macro << "': ignoring it";
    found_macro = end_macro = end_macro + 1;
  }
  std::string substr = naming_scheme.substr(
                         end_macro,
                         found_macro - end_macro);
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
 *  Get a member of the data.
 *
 *  @param[in] d    The data.
 *  @param[out] is  The stream.
 */
template <typename T, typename U, T (U::*member)>
void query::_get_member(io::data const& d, std::ostream& is) {
  is << static_cast<U const&>(d).*member;
}

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
 *  @param[in] d   Unused.
 *  @param[in] is  The stream.
 */
void query::_get_dollar_sign(io::data const& d, std::ostream& is) {
  (void)d;
  is << "$";
  return ;
}
/**
 *  Get the status index id of a data, be it either metric or status.
 *
 *  @param[in] d  The data.
 *
 *  @return       The index id.
 */
unsigned int query::_get_index_id(io::data const& d) {
  if (_type == status)
    return (static_cast<storage::status const&>(d).index_id);
  else if (_type == metric)
    return (_cache->get_metric_mapping(
              static_cast<storage::metric const&>(d).metric_id).index_id);
  return (0);
}

/**
 *  Get the status index id of a data, be it either metric or status.
 *
 *  @param[in] d    The data.
 *  @param[out] is  The stream.
 */
void query::_get_index_id(io::data const& d, std::ostream& is) {
  is << _get_index_id(d);
}

/**
 *  Get the name of a host.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_host(io::data const& d, std::ostream& is) {
  unsigned int index_id = _get_index_id(d);
  is << _cache->get_host_name(_cache->get_index_mapping(index_id).host_id);
}

/**
 *  Get the id of a host.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_host_id(io::data const& d, std::ostream& is) {
  unsigned int index_id = _get_index_id(d);
  is << _cache->get_index_mapping(index_id).host_id;
}

/**
 *  Get the name of a service.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_service(io::data const& d, std::ostream& is) {
  unsigned int index_id = _get_index_id(d);
  storage::index_mapping const& stm = _cache->get_index_mapping(index_id);
  is << _cache->get_service_description(stm.host_id, stm.service_id);
}

/**
 *  Get the id of a service.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_service_id(io::data const& d, std::ostream& is) {
  unsigned int index_id = _get_index_id(d);
  is << _cache->get_index_mapping(index_id).service_id;
}

/**
 *  Get the name of an instance.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_instance(io::data const& d, std::ostream& is) {
  is << _cache->get_instance(d.source_id);
}

