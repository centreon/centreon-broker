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

#ifndef CCB_NEB_CUSTOM_VARIABLE_HH_
# define CCB_NEB_CUSTOM_VARIABLE_HH_

# include "com/centreon/broker/neb/custom_variable_status.hh"

namespace                  com {
  namespace                centreon {
    namespace              broker {
      namespace            neb {
        /**
         *  @class custom_variable custom_variable.hh "com/centreon/broker/neb/custom_variable.hh"
         *  @brief Custom variable definition.
         *
         *  Nagios allows users to declare variables attached to a
         *  specific host or service.
         */
        class              custom_variable : public custom_variable_status {
         private:
          void             _internal_copy(custom_variable const& cv);

         public:
          short            var_type;
                           custom_variable();
                           custom_variable(custom_variable const& cv);
                           ~custom_variable();
          custom_variable& operator=(custom_variable const& cv);
          QString const&   type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_CUSTOM_VARIABLE_HH_ */
