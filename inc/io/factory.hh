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

#ifndef CCB_IO_FACTORY_HH_
# define CCB_IO_FACTORY_HH_

# include "endpoint.hh"

namespace                  com {
  namespace                centreon {
    namespace              broker {
      namespace            io {
        /**
         *  @class factory factory.hh "io/factory.hh"
         *  @brief Endpoint factory.
         *
         *  Build endpoint according to some configuration.
         */
        class              factory {
         public:
                           factory();
                           factory(factory const& f);
          virtual          ~factory();
          factory&         operator=(factory const& f);
          virtual factory* clone() const = 0;
          virtual bool     had_endpoint() const = 0;
          virtual endpoint new_endpoint() const = 0;
        };
      }
    }
  }
}

#endif /* !CCB_IO_FACTORY_HH_ */
