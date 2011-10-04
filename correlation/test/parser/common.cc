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

#include "test/parser/common.hh"

using namespace com::centreon::broker;

/**
 *  Compare two states.
 *
 *  @param[in] n1 First state.
 *  @param[in] n2 Second state.
 *
 *  @return true if both states are equal.
 */
bool compare_states(QMap<QPair<unsigned int, unsigned int>, correlation::node> const& n1,
                    QMap<QPair<unsigned int, unsigned int>, correlation::node> const& n2) {
  bool retval;
  if (n1.size() != n2.size())
    retval = false;
  else {
    retval = true;
    for (QMap<QPair<unsigned int, unsigned int>, correlation::node>::const_iterator
           it1 = n1.begin(),
           end1 = n1.end(),
           it2 = n2.begin();
         retval && (it1 != end1);
         ++it1, ++it2)
      if ((it1.key() != it2.key())
          || (*it1 != *it2))
        retval = false;
  }
  return (retval);
}
