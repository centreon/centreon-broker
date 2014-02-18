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

#ifndef CCB_STATS_BUILDER_HH
#  define CCB_STATS_BUILDER_HH

#  include <string>
#  include "com/centreon/broker/io/properties.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace                 processing {
  class                   failover;
}

namespace                 stats {
  /**
   *  @class builder builder.hh "com/centreon/broker/stats/builder.hh"
   *  @brief Parse a <stats> node.
   */
  class                   builder {
  public:
                          builder();
                          builder(builder const& right);
                          ~builder() throw ();
    builder&              operator=(builder const& right);
    void                  build();
    std::string const&    data() const throw ();
    io::properties const& root() const throw ();

  private:
    static void           _generate_stats_for_endpoint(
                            processing::failover* fo,
                            std::string& buffer,
                            io::properties& tree,
                            bool is_out);
    static void           _serialize(
                            std::string& buffer,
                            io::properties const& tree);

    std::string           _data;
    io::properties        _root;
  };
}

CCB_END()

#endif // !CCB_STATS_BUILDER_HH
