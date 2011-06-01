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

#ifndef CCB_MODULE_SET_LOG_DATA_HH_
# define CCB_MODULE_SET_LOG_DATA_HH_

namespace       com {
  namespace     centreon {
    namespace   broker {
      // Forward declaration.
      namespace events
      { class   log_entry; }

      namespace module {
        void    set_log_data(events::log_entry& le, char const* log_data);
      }
    }
  }
}

#endif /* !CCB_MODULE_SET_LOG_DATA_HH_ */
