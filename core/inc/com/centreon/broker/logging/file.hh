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

#ifndef CCB_LOGGING_FILE_HH
#  define CCB_LOGGING_FILE_HH

#  include <QFile>
#  include "com/centreon/broker/logging/backend.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              logging {
  /**
   *  @class file file.hh "com/centreon/broker/logging/file.hh"
   *  @brief Log messages to a file.
   *
   *  Log messages to a file.
   */
  class                file : public backend {
   public:
                       file(
                         QString const& path,
                         unsigned long long max = 100000000000ull);
                       file(FILE* special);
                       ~file();
    void               log_msg(char const* msg,
                         unsigned int len,
                         type log_type,
                         level l) throw ();
    static bool        with_thread_id();
    static void        with_thread_id(bool enable);
    static bool        with_timestamp();
    static void        with_timestamp(bool enable);

   private:
                       file(file const& f);
    file&              operator=(file const& f);
    void               _internal_copy(file const& f);
    void               _reopen();
    void               _write(char const* data) throw ();
    QFile              _file;
    unsigned long long _max;
    bool               _special;
    static bool        _with_timestamp;
    static bool        _with_thread_id;
    unsigned long long _written;
  };
}

CCB_END()

#endif // !CCB_LOGGING_FILE_HH
