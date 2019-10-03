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

#ifndef CCB_INFLUXDB_LINE_PROTOCOL_QUERY_HH
#define CCB_INFLUXDB_LINE_PROTOCOL_QUERY_HH

#include <string>
#include <utility>
#include <vector>
#include "com/centreon/broker/influxdb/column.hh"
#include "com/centreon/broker/influxdb/macro_cache.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/status.hh"

CCB_BEGIN()

namespace influxdb {
/**
 *  @class line_protocol_query line_protocol_query.hh
 * "com/centreon/broker/graphite/line_protocol_query.hh"
 *  @brief Query compiling/generation.
 *
 *  This class compiles a query for further uses, generating
 *  the query fast.
 */
class line_protocol_query {
 public:
  enum data_type { unknown, metric, status };
  typedef void (line_protocol_query::*data_getter)(io::data const&,
                                                   std::ostream&);
  typedef std::string (line_protocol_query::*data_escaper)(std::string const&);

  line_protocol_query();
  line_protocol_query(std::string const& timeseries,
                      std::vector<column> const& columns,
                      data_type type,
                      macro_cache const& cache);
  line_protocol_query(line_protocol_query const& other);
  ~line_protocol_query();
  line_protocol_query& operator=(line_protocol_query const& other);

  std::string escape_key(std::string const& str);
  std::string escape_measurement(std::string const& str);
  std::string escape_value(std::string const& str);

  std::string generate_metric(storage::metric const& me);
  std::string generate_status(storage::status const& st);

 private:
  void _append_compiled_getter(data_getter getter, data_escaper escaper);
  void _append_compiled_string(std::string const& str,
                               data_escaper escaper = NULL);
  void _compile_scheme(std::string const& scheme, data_escaper escaper);
  void _throw_on_invalid(data_type macro_type);

  template <typename T, typename U, T(U::*member)>
  void _get_member(io::data const& d, std::ostream& is);
  void _get_string(io::data const& d, std::ostream& is);
  void _get_null(io::data const& d, std::ostream& is);
  void _get_dollar_sign(io::data const& d, std::ostream& is);
  uint32_t _get_index_id(io::data const& d);
  void _get_index_id(io::data const& d, std::ostream& is);
  void _get_host(io::data const& d, std::ostream& is);
  void _get_host_id(io::data const& d, std::ostream& is);
  void _get_service(io::data const& d, std::ostream& is);
  void _get_service_id(io::data const& d, std::ostream& is);
  void _get_instance(io::data const& d, std::ostream& is);

  // Compiled data.
  std::vector<std::pair<data_getter, data_escaper> > _compiled_getters;
  std::vector<std::string> _compiled_strings;

  // Used for generation.
  size_t _string_index;
  data_type _type;

  // Macro cache
  macro_cache const* _cache;
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_LINE_PROTOCOL_QUERY_HH
