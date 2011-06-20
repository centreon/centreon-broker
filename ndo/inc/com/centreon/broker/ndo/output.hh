/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NDO_OUTPUT_HH_
# define CCB_NDO_OUTPUT_HH_

# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        ndo {
        /**
         *  @class output output.hh "com/centreon/broker/ndo/output.hh"
         *  @brief NDO output destination.
         *
         *  The class converts events to an output stream using a
         *  modified version of the NDO protocol.
         */
        class          output : virtual public io::stream {
         public:
                       output();
                       output(output const& o);
          virtual      ~output();
          output&      operator=(output const& o);
          virtual QSharedPointer<io::data>
                       read();
          virtual void write(QSharedPointer<io::data> e);
        };
      }
    }
  }
}

#endif /* !CCB_NDO_OUTPUT_HH_ */
