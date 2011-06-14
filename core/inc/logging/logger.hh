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
*/

#ifndef CCB_LOGGING_LOGGER_HH_
# define CCB_LOGGING_LOGGER_HH_

# include "logging/defines.hh"
# include "logging/temp_logger.hh"

namespace             com {
  namespace           centreon {
    namespace         broker {
      namespace       logging {
        /**
         *  @class logger logger.hh "logging/logger.hh"
         *  @brief Log messages.
         *
         *  Messages can be sent in various forms and stored in multiple
         *  facilities like syslog, files or standard output.
         */
        class         logger {
         private:
          type        _type;
                      logger(logger const& l);
          logger&     operator=(logger const& l);

         public:
                      logger(type log_type);
                      ~logger();

          /**
           *  @brief Output an element.
           *
           *  If the type of this element is supported, it will be sent
           *  to configured outputs.
           *
           *  @param[in] t Element to send.
           *
           *  @return Temporary object.
           */
          template    <typename T>
          temp_logger operator<<(T t) throw () {
            return (temp_logger(_type) << t);
          }
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_LOGGER_HH_ */
