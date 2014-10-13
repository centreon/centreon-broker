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

#ifndef CCB_NOTIFICATION_NODE_CACHE_HH
#  define CCB_NOTIFICATION_NODE_CACHE_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/bbdo/stream.hh"
#  include "com/centreon/broker/compression/stream.hh"
#  include "com/centreon/broker/file/stream.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class node_cache node_cache.hh "com/centreon/broker/notification/node_cache.hh"
   *  @brief Contain and update a cache of node information.
   */
  class           node_cache : public io::stream {
  public:
                  node_cache();
                  node_cache(node_cache const& f);
    node_cache&   operator=(node_cache const& f);

    bool          load(std::string const& cache_file);
    bool          unload(std::string const& cache_file);

    virtual void  process(bool in, bool out);
    virtual void  read(misc::shared_ptr<io::data> &d);
    virtual unsigned int
                  write(const misc::shared_ptr<io::data> &d);

  private:
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
