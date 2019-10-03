/*
** Copyright 2015-2017 Centreon
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

#include "com/centreon/broker/graphite/query.hh"
#include <algorithm>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

/**
 *  Constructor.
 *
 *  @param[in] naming_scheme  The naming scheme to use.
 *  @param[in] escape_string  String used to escape special chars
 *                            (especially dot).
 *  @param[in] type           The type of the query: metric or status.
 *  @param[in] cache          The macro cache.
 */
query::query(std::string const& naming_scheme,
             std::string const& escape_string,
             data_type type,
             macro_cache const& cache)
    : _escape_string(escape_string),
      _naming_scheme_index(0),
      _type(type),
      _cache(&cache) {
  _compile_naming_scheme(naming_scheme, type);
}

/**
 *  Copy operator.
 *
 *  @param[in] other  The object to copy.
 */
query::query(query const& other)
    : _compiled_naming_scheme(other._compiled_naming_scheme),
      _compiled_getters(other._compiled_getters),
      _escape_string(other._escape_string),
      _naming_scheme_index(other._naming_scheme_index),
      _type(other._type),
      _cache(other._cache) {}

/**
 *  Destructor
 */
query::~query() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return       A reference to this object.
 */
query& query::operator=(query const& other) {
  if (this != &other) {
    _compiled_naming_scheme = other._compiled_naming_scheme;
    _compiled_getters = other._compiled_getters;
    _escape_string = other._escape_string;
    _naming_scheme_index = other._naming_scheme_index;
    _type = other._type;
    _cache = other._cache;
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
    throw(exceptions::msg() << "graphite: attempt to generate metric"
                               " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  std::ostringstream tmp;
  try {
    for (std::vector<void (query::*)(io::data const&,
                                     std::ostream&)>::const_iterator
             it(_compiled_getters.begin()),
         end(_compiled_getters.end());
         it != end; ++it) {
      (this->**it)(me, tmp);
      std::string escaped = tmp.str();
      misc::string::replace(escaped, " ", "_");
      iss << escaped;
      tmp.str("");
    }
  } catch (std::exception const& e) {
    logging::error(logging::high)
        << "graphite: couldn't generate query for metric " << me.metric_id
        << ":" << e.what();
    return ("");
  }

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
    throw(exceptions::msg() << "graphite: attempt to generate status"
                               " with a query of the bad type");
  _naming_scheme_index = 0;
  std::ostringstream iss;
  std::ostringstream tmp;
  try {
    for (std::vector<void (query::*)(io::data const&,
                                     std::ostream&)>::const_iterator
             it(_compiled_getters.begin()),
         end(_compiled_getters.end());
         it != end; ++it) {
      (this->**it)(st, tmp);
      std::string escaped = tmp.str();
      misc::string::replace(escaped, " ", "_");
      iss << escaped;
      tmp.str("");
    }
  } catch (std::exception const& e) {
    logging::error(logging::high)
        << "graphite: couldn't generate query for status " << st.index_id << ":"
        << e.what();
    return ("");
  }

  iss << (" ") << st.state << " " << st.ctime << "\n";

  return (iss.str());
}

/**
 *  Compile a naming scheme.
 *
 *  @param[in] naming_scheme  The naming scheme to compile.
 *  @param[in] type           The type of this query.
 */
void query::_compile_naming_scheme(std::string const& naming_scheme,
                                   data_type type) {
  (void)type;
  size_t found_macro = 0;
  size_t end_macro = 0;

  while ((found_macro = naming_scheme.find_first_of('$', found_macro)) !=
         std::string::npos) {
    std::string substr =
        naming_scheme.substr(end_macro, found_macro - end_macro);
    if (!substr.empty()) {
      _compiled_naming_scheme.push_back(substr);
      _compiled_getters.push_back(&query::_get_string);
    }

    if ((end_macro = naming_scheme.find_first_of('$', found_macro + 1)) ==
        std::string::npos)
      throw exceptions::msg()
          << "graphite: can't compile query, opened macro not closed: '"
          << naming_scheme.substr(found_macro) << "'";

    std::string macro{
        naming_scheme.substr(found_macro, end_macro + 1 - found_macro)};
    if (macro == "")
      _compiled_getters.push_back(&query::_get_dollar_sign);
    if (macro == "$METRICID$") {
      _throw_on_invalid(metric);
      _compiled_getters.push_back(
          &query::_get_member<uint32_t, storage::metric,
                              &storage::metric::metric_id>);
    } else if (macro == "$INSTANCE$")
      _compiled_getters.push_back(&query::_get_instance);
    else if (macro == "$INSTANCEID$")
      _compiled_getters.push_back(
          &query::_get_member<uint32_t, io::data, &io::data::source_id>);
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
          &query::_get_string_member<storage::metric, &storage::metric::name>);
    } else if (macro == "$INDEXID$") {
      _compiled_getters.push_back(&query::_get_index_id);
    } else
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
 *  Escape data string.
 *
 *  @param[in] str  Base string.
 *
 *  @return Escaped string.
 */
std::string query::_escape(std::string const& str) {
  std::string retval{str};
  size_t pos = retval.find('.');

  while (pos != std::string::npos) {
    retval.replace(pos, 1, _escape_string);
    pos = retval.find('.', pos + _escape_string.size());
  }
  return retval;
}

/**
 *  Throw on invalid macro type.
 *
 *  @param[in] macro_type  The macro type;
 */
void query::_throw_on_invalid(data_type macro_type) {
  if (macro_type != _type)
    throw exceptions::msg() << "graphite: macro of invalid type";
}

/*
 * Getters
 */

/**
 *  Get a member of the data.
 *
 *  @param[in]  d   The data.
 *  @param[out] is  The stream.
 */
template <typename T, typename U, T(U::*member)>
void query::_get_member(io::data const& d, std::ostream& is) {
  is << static_cast<U const*>(&d)->*member;
}

/**
 *  Get a string data member.
 *
 *  @param[in]  d   The data.
 *  @param[out] is  The stream.
 */
template <typename U, std::string(U::*member)>
void query::_get_string_member(io::data const& d, std::ostream& is) {
  is << _escape(static_cast<U const*>(&d)->*member);
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
}

/**
 *  Get the status index id of a data, be it either metric or status.
 *
 *  @param[in] d  The data.
 *
 *  @return       The index id.
 */
uint64_t query::_get_index_id(io::data const& d) {
  if (_type == status)
    return (static_cast<storage::status const&>(d).index_id);
  else if (_type == metric)
    return (_cache
                ->get_metric_mapping(
                    static_cast<storage::metric const&>(d).metric_id)
                .index_id);
  return 0;
}

/**
 *  Get the status index id of a data, be it either metric or status.
 *
 *  @param[in] d    The data.
 *  @param[out] is  The stream
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
  uint64_t index_id{_get_index_id(d)};
  is << _escape(
      _cache->get_host_name(_cache->get_index_mapping(index_id).host_id));
}

/**
 *  Get the id of a host.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_host_id(io::data const& d, std::ostream& is) {
  uint64_t index_id(_get_index_id(d));
  is << _cache->get_index_mapping(index_id).host_id;
}

/**
 *  Get the name of a service.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_service(io::data const& d, std::ostream& is) {
  uint64_t index_id = _get_index_id(d);
  storage::index_mapping const& stm = _cache->get_index_mapping(index_id);
  is << _escape(_cache->get_service_description(stm.host_id, stm.service_id));
}

/**
 *  Get the id of a service.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_service_id(io::data const& d, std::ostream& is) {
  uint64_t index_id(_get_index_id(d));
  is << _cache->get_index_mapping(index_id).service_id;
}

/**
 *  Get the name of an instance.
 *
 *  @param[in] d  The data.
 *  @param is     The stream.
 */
void query::_get_instance(io::data const& d, std::ostream& is) {
  is << _escape(_cache->get_instance(d.source_id));
}
