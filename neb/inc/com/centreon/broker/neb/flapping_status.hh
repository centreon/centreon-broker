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

#ifndef CCB_NEB_FLAPPING_STATUS_HH_
# define CCB_NEB_FLAPPING_STATUS_HH_

# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                  com {
  namespace                centreon {
    namespace              broker {
      namespace            neb {
        /**
         *  @class flapping_status flapping_status.hh "com/centreon/broker/neb/flapping_status.hh"
         *  @brief Store a flapping status.
         *
         *  Store flapping statuses.
         */
        class              flapping_status : public io::data {
         private:
          void             _internal_copy(flapping_status const& fs);

         public:
          time_t           comment_time;
          time_t           event_time;
          int              event_type;
          short            flapping_type;
          double           high_threshold;
          unsigned int     host_id;
          unsigned int     internal_comment_id;
          double           low_threshold;
          double           percent_state_change;
          short            reason_type;
          unsigned int     service_id;
                           flapping_status();
                           flapping_status(flapping_status const& fs);
                           ~flapping_status();
          flapping_status& operator=(flapping_status const& fs);
          QString const&   type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_FLAPPING_STATUS_HH_ */
