/*
** Copyright 2012-2013 Merethis
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

#ifndef TEST_CONFIG_HH
#  define TEST_CONFIG_HH

#  include <list>
#  include <QSqlDatabase>
#  include "com/centreon/engine/objects.hh"

void         config_db_close(char const* db_name);
QSqlDatabase config_db_open(char const* db_name);
void         config_remove(char const* path);
void         config_write(
               char const* path,
               char const* more_directives = NULL,
               std::list<host>* hosts = NULL,
               std::list<service>* services = NULL,
               std::list<command>* commands = NULL,
               std::list<hostgroup>* host_groups = NULL,
               std::list<servicegroup>* service_groups = NULL,
               std::list<hostdependency>* host_deps = NULL,
               std::list<servicedependency>* service_deps = NULL);

#endif // !TEST_CONFIG_HH
