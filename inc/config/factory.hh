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

#ifndef CONFIG_FACTORY_HH_
# define CONFIG_FACTORY_HH_

# include "config/logger.hh"
# include "logging/backend.hh"

namespace             config
{
  namespace           factory
  {
    logging::backend* build(config::logger const& conf);
  }
}

#endif /* !CONFIG_FACTORY_HH_ */
