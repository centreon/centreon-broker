/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_LOGGING_TEMP_LOGGER_HH
#  define CC_LOGGING_TEMP_LOGGER_HH

#  include <string>
#  include "com/centreon/logging/engine.hh"
#  include "com/centreon/namespace.hh"
#  include "com/centreon/misc/stringifier.hh"

CC_BEGIN()

namespace                     logging {
    struct                    setprecision {
                              setprecision(int val = -1)
                                : precision(val) {}
      int                     precision;
    };

  /**
   *  @class temp_logger temp_logger.hh "com/centreon/logging/temp_logger.hh"
   *  @brief Log messages.
   *
   *  Used to buffering log messages before writing them into backends.
   */
  class                       temp_logger {
  public:
                              temp_logger(
                                unsigned long long types,
                                unsigned int verbose) throw ();
                              temp_logger(temp_logger const& right);
    virtual                   ~temp_logger() throw ();
    temp_logger&              operator=(temp_logger const& right);
    temp_logger&              operator<<(setprecision const& obj) throw ();
    template<typename T>
    temp_logger&              operator<<(T obj) throw () {
      _buffer << obj;
      return (*this);
    }

  private:
    temp_logger&              _internal_copy(temp_logger const& right);

    misc::stringifier         _buffer;
    engine&                   _engine;
    unsigned long long        _type;
    unsigned int              _verbose;
  };
}

CC_END()

#endif // !CC_LOGGING_TEMP_LOGGER_HH
