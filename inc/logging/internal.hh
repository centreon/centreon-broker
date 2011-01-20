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

#ifndef LOGGING_INTERNAL_HH_
# define LOGGING_INTERNAL_HH_

# include <map>
# include <memory>
# include <utility>
# include "logging/backend.hh"

namespace logging {
  extern std::map<backend*, std::pair<unsigned int, level> > backends;
}

#endif /* !LOGGING_INTERNAL_HH_ */
