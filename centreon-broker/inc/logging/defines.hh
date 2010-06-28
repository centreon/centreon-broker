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

#ifndef LOGGING_DEFINES_HH_
# define LOGGING_DEFINES_HH_

namespace logging
{
  // Log levels.
  enum    level
  {
    NONE = 0,
    HIGH,
    MEDIUM,
    LOW
  };

  // Log types.
  enum    type
  {
    CONFIG = 1,
    DEBUG = 2,
    ERROR = 4,
    INFO = 8
  };
}

#endif /* !LOGGING_DEFINES_HH_ */
