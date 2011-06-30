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

#ifndef CCB_LOGGING_FILE_HH_
# define CCB_LOGGING_FILE_HH_

# include <QFile>
# include "com/centreon/broker/logging/backend.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         logging {
        /**
         *  @class file file.hh "com/centreon/broker/logging/file.hh"
         *  @brief Log messages to a file.
         *
         *  Log messages to a file.
         */
        class         file : public backend {
         private:
          QFile       _file;
          bool        _special;
          static bool _with_timestamp;
                      file(file const& f);
          file&       operator=(file const& f);
          void        _write(char const* data) throw ();

         public:
                      file(QString const& path);
                      file(FILE* special);
                      ~file();
          void        log_msg(char const* msg,
                        unsigned int len,
                        type log_type,
                        level l) throw ();
          static bool with_timestamp();
          static void with_timestamp(bool enable);
        };
      }
    }
  }
}

#endif /* !CCB_LOGGING_FILE_HH_ */
