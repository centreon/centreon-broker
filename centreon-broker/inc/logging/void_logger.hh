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

#ifndef LOGGING_VOID_LOGGER_HH_
# define LOGGING_VOID_LOGGER_HH_

namespace        logging
{
  /**
   *  @class void_logger void_logger.hh "logging/void_logger.hh"
   *  @brief Log nothing.
   *
   *  Used for compatibility if a message type is deactivated (debug messages
   *  for example).
   */
  class          void_logger
  {
   public:
                 void_logger();
                 void_logger(void_logger const& v);
                 ~void_logger();
    void_logger& operator=(void_logger const& v);

    /**
     *  Log nothing (compatibility method).
     *
     *  @param[in] t Unused.
     *
     *  @return Current instance.
     */
    template     <typename T>
    void_logger& operator<<(T t) throw ()
    {
      (void)t;
      return (*this);
    }
  };
}

#endif /* !LOGGING_VOID_LOGGER_HH_ */
