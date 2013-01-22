/*
** Copyright 2011 Merethis
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

#include <cstring>
#include <QString>
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that mixed insertions works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Insert data.
  misc::stringifier s;
  s << 147852l << 894567891023ull << "foobar" << -425789751234547ll
    << QString("baz   qux") << false << -42 << "2156" << true;

  // Check resulting string.
  return (strcmp(s.data(), "147852894567891023foobar-425789751234547b" \
                           "az   quxfalse-422156true"));
}
