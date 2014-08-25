/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_QHASH_FUNC_HH
#  define CCB_NOTIFICATION_QHASH_FUNC_HH

#  include <string>
#  include <QtGlobal>
#  include "com/centreon/broker/namespace.hh"

// QHash function for std::string
uint qHash(std::string const& str);

#endif // !CCB_NOTIFICATION_QHASH_FUNC_HH
