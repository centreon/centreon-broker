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

#ifndef LOGGING_TEMP_LOGGER_HH_
# define LOGGING_TEMP_LOGGER_HH_

# include "logging/defines.hh"
# include "misc/stringifier.hh"

namespace        logging
{
  /**
   *  @class temp_logger temp_logger.hh "logging/temp_logger.hh"
   *  @brief Log messages.
   *
   *  This class is used to temporarily buffer log messages before writing them
   *  to backends.
   */
  class          temp_logger : private misc::stringifier
  {
   private:
    mutable bool _copied;
    level        _level;
    type         _type;
    void         _internal_copy(temp_logger const& t);

   public:
                 temp_logger(type log_type);
                 temp_logger(temp_logger const& t);
                 ~temp_logger();
    temp_logger& operator=(temp_logger const& t);
    temp_logger& operator<<(bool b) throw ();
    temp_logger& operator<<(double d) throw ();
    temp_logger& operator<<(int i) throw ();
    temp_logger& operator<<(unsigned int i) throw ();
    temp_logger& operator<<(char const* str) throw ();
    temp_logger& operator<<(level l) throw ();
  };
}

#endif /* !LOGGING_TEMP_LOGGER_HH_ */
