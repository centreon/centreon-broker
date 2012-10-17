/*
** Copyright 2012 Merethis
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

#ifndef TEST_GENERATE_HH
#  define TEST_GENERATE_HH

#  include <list>
#  include "com/centreon/engine/objects.hh"

void free_hosts(std::list<host>& hosts);
void free_services(std::list<service>& services);
void generate_hosts(
       std::list<host>& hosts,
       unsigned int count);
void generate_services(
       std::list<service>& services,
       std::list<host>& hosts,
       unsigned int services_per_host);

#endif // !TEST_GENERATE_HH
