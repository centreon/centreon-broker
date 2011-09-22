/*
** Copyright 2011 Merethis
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

#ifndef CCB_CORRELATION_ENGINE_HH_
# define CCB_CORRELATION_ENGINE_HH_

# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          correlation {
        /**
         *  @class engine engine.hh "com/centreon/broker/correlation/engine.hh"
         *  @brief Correlation engine event.
         *
         *  Notify of correlation engine event.
         */
        class            engine : public io::data {
         public:
          bool           activated;
                         engine();
                         engine(engine const& e);
                         ~engine();
          engine&        operator=(engine const& e);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_CORRELATION_ENGINE_HH_ */
