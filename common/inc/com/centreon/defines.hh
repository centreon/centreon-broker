/*
** Copyright 2014 Merethis
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

#ifndef CCB_BROKER_DEFINES_HH
#define CCB_BROKER_DEFINES_HH



namespace com{
  namespace centreon{
    namespace broker{

      enum e_service_state{
        E_STATE_OK      = 0,
        E_STATE_WARNING = 1,
        E_STATE_CRITICAL= 2,
        E_STATE_UNKNOWN = 3,
        E_STATE_PENDING = 4 };




    }
  }
}

#endif
