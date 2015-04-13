/*
** Copyright 2009-2015 Merethis
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

#ifndef CCB_SQL_INTERNAL_HH
#  define CCB_SQL_INTERNAL_HH

#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/io/data.hh"

com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::io::data const& d);

#endif // !CCB_SQL_INTERNAL_HH
