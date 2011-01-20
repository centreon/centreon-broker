/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef CONFIG_GLOBALS_HH_
# define CONFIG_GLOBALS_HH_

# include <string>

namespace              config {
  namespace            globals {
    extern bool        correlation;
    extern std::string correlation_file;
    extern int         instance;
    extern std::string instance_name;
  }
}

#endif /* !CONFIG_GLOBALS_HH_ */
