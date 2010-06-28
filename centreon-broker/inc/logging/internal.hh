/*
** This file is part of Centreon Dhana.
**
** Centreon Dhana is free software: you can redistribute it and/or modify it
** under the terms of the GNU Affero General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** Centreon Dhana is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
** License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with Centreon Dhana. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LOGGING_INTERNAL_HH_
# define LOGGING_INTERNAL_HH_

# include <map>
# include <memory>
# include <utility>
# include "logging/backend.hh"

namespace logging
{
  extern std::map<backend*, std::pair<unsigned int, level> > backends;
}

#endif /* !LOGGING_INTERNAL_HH_ */
