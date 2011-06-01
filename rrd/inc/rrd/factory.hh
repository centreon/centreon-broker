/*
** Copyright 2011 Merethis
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

#ifndef CCB_RRD_FACTORY_HH_
# define CCB_RRD_FACTORY_HH_

# include "io/factory.hh"

namespace          com {
  namespace        centreon {
    namespace      broker {
      namespace    rrd {
        /**
         *  @class factory factory.hh "ndo/factory.hh"
         *  @brief RRD layer factory.
         *
         *  Build RRD layer objects.
         */
        class      factory : public com::centreon::broker::io::factory {
         public:
                   factory();
                   factory(factory const& f);
                   ~factory();
          factory& operator=(factory const& f);
          com::centreon::broker::io::factory*
                   clone() const;
          bool     has_endpoint(com::centreon::broker::config::endpoint const& cfg,
                     bool is_input,
                     bool is_output) const;
          com::centreon::broker::io::endpoint*
                   new_endpoint(com::centreon::broker::config::endpoint const& cfg,
                     bool is_input,
                     bool is_output,
                     bool& is_acceptor) const;
        };
      }
    }
  }
}

#endif /* !CCB_RRD_FACTORY_HH_ */
