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

#ifndef CCB_IO_FACTORY_HH
#  define CCB_IO_FACTORY_HH

#  include "com/centreon/broker/config/endpoint.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace             io {
  /**
   *  @class factory factory.hh "com/centreon/broker/io/factory.hh"
   *  @brief Endpoint factory.
   *
   *  Build endpoint according to some configuration.
   */
  class               factory {
  public:
                      factory();
                      factory(factory const& f);
    virtual           ~factory();
    factory&          operator=(factory const& f);
    virtual factory*  clone() const = 0;
    virtual bool      has_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output) const = 0;
    virtual bool      has_not_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output) const;
    virtual endpoint* new_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output,
                        bool& is_acceptor,
                        misc::shared_ptr<persistent_cache> cache = misc::shared_ptr<persistent_cache>()) const = 0;
    virtual misc::shared_ptr<stream>
                      new_stream(
                        misc::shared_ptr<stream> to,
                        bool is_acceptor,
                        QString const& proto_name);
  };
}

CCB_END()

#endif // !CCB_IO_FACTORY_HH
