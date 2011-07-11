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

#ifndef CCB_RRD_EXCEPTIONS_OPEN_HH_
# define CCB_RRD_EXCEPTIONS_OPEN_HH_

# include "com/centreon/broker/exceptions/msg.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          rrd {
        namespace        exceptions {
          /**
           *  @class open open.hh "com/centreon/broker/rrd/exceptions/open.hh"
           *  @brief RRD file open error.
           *
           *  Exception thrown when unable to open an RRD file.
           */
          class          open : public broker::exceptions::msg {
           public:
                         open() throw ();
                         open(open const& o) throw ();
                         ~open() throw ();
            open&        operator=(open const& o) throw ();
            virtual broker::exceptions::msg*
                         clone() const;
            virtual void rethrow() const;

            /**
             *  Insert data in message.
             *
             *  @param[in] t Data to insert.
             */
            template     <typename T>
            open&        operator<<(T t) throw () {
              broker::exceptions::msg::operator<<(t);
              return (*this);
            }
          };
        }
      }
    }
  }
}

#endif /* !CCB_RRD_EXCEPTIONS_OPEN_HH_ */
