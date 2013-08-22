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

#ifndef CCB_MISC_DIAGNOSTIC_HH
#  define CCB_MISC_DIAGNOSTIC_HH

#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       misc {
  /**
   *  @class diagnostic diagnostic.hh "com/centreon/broker/misc/diagnostic.hh"
   *  @brief Generate diagnostic files.
   *
   *  Generate diagnostic files to resolve Centreon Broker issues.
   */
  class         diagnostic {
  public:
                diagnostic();
                diagnostic(diagnostic const& right);
                ~diagnostic() throw ();
    diagnostic& operator=(diagnostic const& right);
    void        generate(
                  std::vector<std::string> const& cfg_files,
                  std::string const& out_file = "");
  };
}

CCB_END()

#endif // !CCB_MISC_DIAGNOSTIC_HH
