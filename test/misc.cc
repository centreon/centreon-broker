/*
** Copyright 2012-2013,2015 Merethis
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

#include <cstdlib>
#include <ctime>
#include <QDir>
#include <QStringList>
#include <sstream>
#include <unistd.h>
#include "test/misc.hh"
#include "test/vars.hh"

/**
 *  Recursively remove a directory.
 *
 *  @param[in] dir_path Target directory.
 */
void recursive_remove(std::string const& dir_path) {
  QStringList entries(QDir(dir_path.c_str()).entryList());
  for (QStringList::const_iterator
         it(entries.begin()),
         end(entries.end());
       it != end;
       ++it) {
    std::ostringstream oss;
    oss << dir_path << "/" << it->toStdString();
    ::remove(oss.str().c_str());
  }
  QDir().rmdir(dir_path.c_str());
  return ;
}

/**
 *  Sleep for some seconds.
 *
 *  @param[in] units  Number of time units to sleep.
 */
void sleep_for(unsigned int units) {
  time_t now(time(NULL));
  time_t target(now + units * MONITORING_ENGINE_INTERVAL_LENGTH);
  while (now < target) {
    sleep(target - now);
    now = time(NULL);
  }
  return ;
}
