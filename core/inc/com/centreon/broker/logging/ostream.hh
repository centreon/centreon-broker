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

#ifndef CCB_LOGGING_OSTREAM_HH_
# define CCB_LOGGING_OSTREAM_HH_

# include <ostream>
# include "com/centreon/broker/logging/backend.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         logging {
        /**
         *  @class ostream ostream.hh "com/centreon/broker/logging/ostream.hh"
         *  @brief Log messages to standard output streams.
         *
         *  Log messages to standard output stream objects, like
         *  std::cout or std::cerr.
         *
         *  @see com::centreon::broker::logging::file
         */
        class           ostream : public backend {
         protected:
          std::ostream* _os;
          static bool   _with_timestamp;

         public:
                        ostream();
                        ostream(ostream const& o);
                        ostream(std::ostream& os);
          virtual       ~ostream();
          ostream&      operator=(ostream const& o);
          ostream&      operator=(std::ostream& os);
          void          log_msg(char const* msg,
                          unsigned int len,
                          type log_type,
                          level l) throw ();
          static void   with_timestamp(bool enable);
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_OSTREAM_HH_ */
