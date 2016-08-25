/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_INFLUXDB_QUERY_HH
#  define CCB_INFLUXDB_QUERY_HH

#  include <utility>
#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"
#  include "com/centreon/broker/influxdb/macro_cache.hh"

std::ostream& operator<<(std::ostream& in, QString const& string);

CCB_BEGIN()

namespace         influxdb {
  /**
   *  @class query query.hh "com/centreon/broker/graphite/query.hh"
   *  @brief Query compiling/generation.
   *
   *  This class compiles a query for further uses, generating
   *  the query fast.
   */
  class           query {
  public:
  public:
    enum          data_type {
                  unknown,
                  metric,
                  status
    };
                  query();
                  query(
                    std::string const& naming_scheme,
                    data_type type,
                    macro_cache const& cache,
                    bool escape = false);
                  query(query const& f);
                  ~query();
    query&        operator=(query const& f);

    std::string   generate_metric(storage::metric const& me);
    std::string   generate_status(storage::status const& st);

  private:
    // Compiled data.
    std::vector<std::string>
                  _compiled_naming_scheme;
    std::vector<void (query::*)(io::data const&, std::ostream&)>
                  _compiled_getters;

    // Used for generation.
    size_t        _naming_scheme_index;
    data_type     _type;

    // Macro cache
    macro_cache const*
                  _cache;

    // Should the data be escaped?
    bool          _escape;

    void          _compile_naming_scheme(
                    std::string const& naming_scheme,
                    data_type type);
    void          _throw_on_invalid(data_type macro_type);

    template <typename T, typename U, T (U::*member)>
    void          _get_member(io::data const& d, std::ostream& is);
    void          _get_string(io::data const& d, std::ostream& is);
    void          _get_null(io::data const& d, std::ostream& is);
    void          _get_dollar_sign(io::data const& d, std::ostream& is);
    unsigned int  _get_index_id(io::data const& d);
    void          _get_index_id(io::data const& d, std::ostream& is);
    void          _get_host(io::data const& d, std::ostream& is);
    void          _get_host_id(io::data const& d, std::ostream& is);
    void          _get_service(io::data const& d, std::ostream& is);
    void          _get_service_id(io::data const& d, std::ostream& is);
    void          _get_instance(io::data const& d, std::ostream& is);
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_QUERY_HH
