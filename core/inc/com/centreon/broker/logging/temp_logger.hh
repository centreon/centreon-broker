/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_LOGGING_TEMP_LOGGER_HH_
# define CCB_LOGGING_TEMP_LOGGER_HH_

# include "com/centreon/broker/logging/defines.hh"
# include "com/centreon/broker/misc/stringifier.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        logging {
        /**
         *  @class temp_logger temp_logger.hh "com/centreon/broker/logging/temp_logger.hh"
         *  @brief Log messages.
         *
         *  This class is used to temporarily buffer log messages before
         *  writing them to backends.
         */
        class          temp_logger : private misc::stringifier {
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
          temp_logger& operator<<(level l) throw ();

          /**
           *  Insert data in message.
           *
           *  @param[in] t Data to insert.
           */
          template     <typename T>
          temp_logger& operator<<(T t) throw () {
            misc::stringifier::operator<<(t);
            return (*this);
          }
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_TEMP_LOGGER_HH_ */
