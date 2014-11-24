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
#  include <map>
#  include <deque>
#  include <QMutex>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/bbdo/stream.hh"
#  include "com/centreon/broker/compression/stream.hh"
#  include "com/centreon/broker/file/stream.hh"
#  include "com/centreon/broker/notification/object_cache.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/host_status.hh"
#  include "com/centreon/broker/neb/host_group_member.hh"
#  include "com/centreon/broker/neb/service_group_member.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class node_cache node_cache.hh "com/centreon/broker/notification/node_cache.hh"
   *  @brief Contain and update a cache of node information.
   *
   *  Used by and for the macro processing. Load from a file at startup,
   *  unload at shutdown.
   */
  class           node_cache : public io::stream {
  public:
    typedef object_cache<neb::host, neb::host_status, neb::host_group_member>
                  host_node_state;
    typedef object_cache<neb::service, neb::service_status, neb::service_group_member>
                  service_node_state;

                  node_cache();
                  node_cache(node_cache const& f);
    node_cache&   operator=(node_cache const& f);

    bool          load(std::string const& cache_file);
    bool          unload(std::string const& cache_file);

    virtual void  process(bool in, bool out);
    virtual void  read(misc::shared_ptr<io::data> &d);
    virtual unsigned int
                  write(const misc::shared_ptr<io::data> &d);

    void          update(neb::host const&);
    void          update(neb::host_status const&);
    void          update(neb::host_group_member const&);
    void          update(neb::service const&);
    void          update(neb::service_status const&);
    void          update(neb::service_group_member const&);


    host_node_state const&
                  get_host(unsigned int id) const;
    service_node_state const&
                  get_service(unsigned int id) const;

  private:
    std::map<unsigned int, host_node_state>
                  _host_node_states;
    std::map<unsigned int, service_node_state>
                  _service_node_states;

    QMutex        _mutex;

    std::deque<misc::shared_ptr<io::data> >
                  _serialized_data;

    void          _prepare_serialization();
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
