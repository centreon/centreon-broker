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

#ifndef CCB_GRAPHITE_QUERY_HH
#  define CCB_GRAPHITE_QUERY_HH

#  include <string>
#  include <map>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"

CCB_BEGIN()

namespace         graphite {
  /**
   *  @class query query.hh "com/centreon/broker/graphite/query.hh"
   *  @brief Query compiling/generation.
   */
  class           query {
  public:
                  query(std::string const& naming_scheme);
                  query(query const& f);
                  ~query();
    query&        operator=(query const& f);

    std::string   generate_metric(storage::metric const& me);
    std::string   generate_status(storage::status const& st);
  private:

    std::string   _naming_scheme;

    void          _one_pass_replace(
                    std::string& ret,
                    std::map<std::string, std::string> const& macros);
  };
}

CCB_END()

#endif // !CCB_GRAPHITE_QUERY_HH
