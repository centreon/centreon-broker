/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_INFLUXDB_QUERY_HH
#  define CCB_INFLUXDB_QUERY_HH

#  include <utility>
#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"

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
                  query();
                  query(std::string const& naming_scheme);
                  query(query const& f);
                  ~query();
    query&        operator=(query const& f);

    std::string   generate_metric(storage::metric const& me);
    std::string   generate_status(storage::status const& st);

  private:
    // Compiled data.
    std::vector<std::string>
                  _compiled_naming_scheme;
    std::vector<std::string (query::*)(io::data const&)>
                  _compiled_getters;

    // Used for generation.
    size_t        _naming_scheme_index;
    enum          data_type {
      metric,
      status
    };
    data_type     _type;

    void          _compile_naming_scheme(std::string const& naming_scheme);

    std::string   _get_string(io::data const& d);
    std::string   _get_metric_id(io::data const& d);
    std::string   _get_metric(io::data const& d);
    std::string   _get_index_id(io::data const& d);
    std::string   _get_instance_id(io::data const& d);
    std::string   _get_instance(io::data const& d);
    std::string   _get_host_id(io::data const& d);
    std::string   _get_host(io::data const& d);
    std::string   _get_service_id(io::data const& d);
    std::string   _get_service(io::data const& d);
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_QUERY_HH
