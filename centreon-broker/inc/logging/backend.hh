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

#ifndef LOGGING_BACKEND_HH_
# define LOGGING_BACKEND_HH_

# include "logging/defines.hh"

namespace        logging
{
  /**
   *  @class backend backend.hh "logging/backend.hh"
   *  @brief Interface for log backends.
   *
   *  In Dhana, multiple backends are available for log writing. This can
   *  either be plain-text files, syslog or standard outputs. This class
   *  defines an interface to communicate with any of these backends.
   *
   *  @see file_backend
   *  @see standard_backend
   *  @see syslog_backend
   */
  class          backend
  {
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

#endif /* !LOGGING_BACKEND_HH_ */
