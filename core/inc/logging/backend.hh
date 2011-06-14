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

#ifndef CCB_LOGGING_BACKEND_HH_
# define CCB_LOGGING_BACKEND_HH_

# include "logging/defines.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        logging {
        /**
         *  @class backend backend.hh "logging/backend.hh"
         *  @brief Interface for log backends.
         *
         *  In Broker, multiple backends are available for log writing.
         *  This can either be plain-text files, syslog or standard
         *  outputs. This class defines an interface to communicate with
         *  any of these backends.
         *
         *  @see file_backend
         *  @see standard_backend
         *  @see syslog_backend
         */
        class          backend {
         public:
                       backend();
                       backend(backend const& b);
          virtual      ~backend();
          backend&     operator=(backend const& b);
          virtual void log_msg(char const* msg,
                         unsigned int len,
                         type log_type,
                         level l) throw () = 0;
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_BACKEND_HH_ */
