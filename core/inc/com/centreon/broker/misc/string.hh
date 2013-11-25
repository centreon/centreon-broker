/*
** Copyright 2013 Merethis
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

#ifndef CCB_MISC_STRING_HH
#  define CCB_MISC_STRING_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        misc {
  namespace      string {
    std::string& replace(
                   std::string& str,
                   std::string const& old_str,
                   std::string const& new_str) {
      std::size_t pos(str.find(old_str, 0));
      while (pos != std::string::npos) {
        str.replace(pos, old_str.size(), new_str);
        pos = str.find(old_str, pos + new_str.size());
      }
      return (str);
    }
  }
}

CCB_END()

#endif // !CCB_MISC_STRING_HH
