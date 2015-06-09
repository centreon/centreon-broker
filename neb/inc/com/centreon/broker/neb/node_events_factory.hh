/*
** Copyright 2015 Merethis
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

#ifndef CCB_NEB_NODE_EVENTS_FACTORY_HH
#  define CCB_NEB_NODE_EVENTS_FACTORY_HH

#  include "com/centreon/broker/io/factory.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         neb {
  /**
   *  @class node_events_factory node_events_factory.hh "com/centreon/broker/neb/node_events_factory.hh"
   *  @brief Node events factory.
   *
   *  Build node events objects.
   */
  class           node_events_factory : public io::factory {
  public:
                  node_events_factory();
                  node_events_factory(node_events_factory const& other);
                  ~node_events_factory();
    node_events_factory&
                  operator=(node_events_factory const& other);
    io::factory*  clone() const;
    bool          has_endpoint(config::endpoint& cfg) const;
    io::endpoint* new_endpoint(
                    config::endpoint& cfg,
                    bool& is_acceptor,
                    misc::shared_ptr<persistent_cache> cache
                    = misc::shared_ptr<persistent_cache>()) const;
  };
}

CCB_END()

#endif // !CCB_NEB_NODE_EVENTS_FACTORY_HH
