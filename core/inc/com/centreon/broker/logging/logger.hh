/*
** Copyright 2009-2012 Merethis
**
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

#ifndef CCB_LOGGING_LOGGER_HH
#  define CCB_LOGGING_LOGGER_HH

#  include "com/centreon/broker/logging/defines.hh"
#  include "com/centreon/broker/logging/temp_logger.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       logging {
  /**
   *  @class logger logger.hh "com/centreon/broker/logging/logger.hh"
   *  @brief Log messages.
   *
   *  Messages can be sent in various forms and stored in multiple
   *  facilities like syslog, files or standard output.
   */
  class         logger {
  public:
                logger(type log_type);
                ~logger();
    temp_logger operator()(level l) throw ();

  private:
                logger(logger const& l);
    logger&     operator=(logger const& l);

    type        _type;
  };
}

CCB_END()

#endif // !CCB_LOGGING_LOGGER_HH
