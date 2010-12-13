/*
**  Copyright 2010 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef LOGGING_LOGGING_HH_
# define LOGGING_LOGGING_HH_

# include <memory>
# include "logging/backend.hh"
# include "logging/logger.hh"
# include "logging/void_logger.hh"

namespace            logging
{
  // Standard logging objects.
  extern logger      config;
# ifdef NDEBUG
  extern void_logger debug;
# else
  extern logger      debug;
# endif /* NDEBUG */
  extern logger      error;
  extern logger      info;

  void               clear();
  void               log_on(backend* b,
                       unsigned int types = CONFIG | DEBUG | ERROR | INFO,
                       level min_priority = HIGH);
}

#endif /* !LOGGING_LOGGING_HH_ */
