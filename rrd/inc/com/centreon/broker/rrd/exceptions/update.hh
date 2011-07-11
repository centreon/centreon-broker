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

#ifndef CCB_RRD_EXCEPTIONS_UPDATE_HH_
# define CCB_RRD_EXCEPTIONS_UPDATE_HH_

# include "com/centreon/broker/exceptions/msg.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          rrd {
        namespace        exceptions {
          /**
           *  @class update update.hh "com/centreon/broker/rrd/exceptions/update.hh"
           *  @brief Update error.
           *
           *  Exception thrown when unable to update an RRD file.
           */
          class          update : public broker::exceptions::msg {
           public:
                         update() throw ();
                         update(update const& u) throw ();
                         ~update() throw ();
            update&      operator=(update const& u) throw ();
            virtual broker::exceptions::msg*
                         clone() const;
            virtual void rethrow() const;

            /**
             *  Insert data in message.
             *
             *  @param[in] t Data to insert.
             */
            template     <typename T>
            update&      operator<<(T t) throw () {
              broker::exceptions::msg::operator<<(t);
              return (*this);
            }
          };
        }
      }
    }
  }
}

#endif /* !CCB_RRD_EXCEPTIONS_UPDATE_HH_ */
