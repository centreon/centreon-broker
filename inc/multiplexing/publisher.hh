/*
** Copyright 2009-2011 MERETHIS
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

#ifndef CCB_MULTIPLEXING_PUBLISHER_HH_
# define CCB_MULTIPLEXING_PUBLISHER_HH_

namespace             com {
  namespace           centreon {
    namespace         broker {
      namespace       multiplexing {
        /**
         *  @class publisher publisher.hh "multiplexing/publisher.hh"
         *  @brief Publish events to registered subscribers.
         *
         *  A publisher object broadcast an event sent to it to every
         *  subscriber.
         *
         *  @see subscriber
         */
        class         publisher {
         public:
                      publisher();
                      publisher(publisher const& p);
                      ~publisher();
          publisher&  operator=(publisher const& p);
          void        close();
          void        event(events::event* e);
        };
      }
    }
  }
}

#endif /* !CCB_MULTIPLEXING_PUBLISHER_HH_ */
