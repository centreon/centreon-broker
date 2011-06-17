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

#ifndef CCB_NEB_CUSTOM_VARIABLE_STATUS_HH_
# define CCB_NEB_CUSTOM_VARIABLE_STATUS_HH_

# include <QString>
# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                         com {
  namespace                       centreon {
    namespace                     broker {
      namespace                   neb {
        /**
         *  @class custom_variable_status custom_variable_status.hh "com/centreon/broker/neb/custom_variable_status.hh"
         *  @brief Custom variable update.
         *
         *  The value of a custom variable has changed.
         */
        class                     custom_variable_status : public io::data {
         private:
          void                    _internal_copy(custom_variable_status const& cvs);

         public:
          unsigned int            host_id;
          bool                    modified;
          QString                 name;
          unsigned int            service_id;
          time_t                  update_time;
          QString                 value;
                                  custom_variable_status();
                                  custom_variable_status(custom_variable_status const& cvs);
          virtual                 ~custom_variable_status();
          custom_variable_status& operator=(custom_variable_status const& cvs);
          virtual QString const&  type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_CUSTOM_VARIABLE_STATUS_HH_ */
