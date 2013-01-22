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

#ifndef PARSER_COMMON_HH
#  define PARSER_COMMON_HH

#  include <QMap>
#  include <QPair>
#  include "com/centreon/broker/correlation/node.hh"

void compare_states(
       QMap<QPair<unsigned int, unsigned int>,
         com::centreon::broker::correlation::node> const& n1,
       QMap<QPair<unsigned int, unsigned int>,
         com::centreon::broker::correlation::node> const& n2);

#endif // !PARSER_COMMON_HH
