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

#include <ctime>
#include <unistd.h>
#include "test/misc.hh"

/**
 *  Sleep for some seconds.
 *
 *  @param[in] seconds Number of seconds to sleep.
 */
void sleep_for(unsigned int seconds) {
  time_t now(time(NULL));
  time_t target(now + seconds);
  while (now < target) {
    sleep(target - now);
    now = time(NULL);
  }
  return ;
}
