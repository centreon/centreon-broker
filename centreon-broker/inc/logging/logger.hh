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

#ifndef LOGGING_LOGGER_HH_
# define LOGGING_LOGGER_HH_

# include "logging/defines.hh"
# include "logging/temp_logger.hh"

namespace        logging
{
  /**
   *  @class logger logger.hh "logging/logger.hh"
   *  @brief Log messages.
   *
   *  Messages can be sent in various forms and stored in multiple facilities
   *  like syslog, files or standard output.
   */
  class          logger
  {
   private:
    type         _type;
                 logger(logger const& l);
    logger&      operator=(logger const& l);

   public:
                 logger(type log_type);
                 ~logger();

    /**
     *  @brief Output an element.
     *
     *  If the type of this element is supported, it will be sent to configured
     *  outputs.
     *
     *  @param[in] t Element to send.
     *
     *  @return Temporary object.
     */
    template     <typename T>
    temp_logger  operator<<(T t) throw ()
    {
      return (temp_logger(_type) << t);
    }
  };
}

#endif /* !LOGGING_LOGGER_HH_ */
