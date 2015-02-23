/*
** Copyright 2012 Merethis
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

#ifndef CCB_FILE_CFILE_HH
#  define CCB_FILE_CFILE_HH

#  include <cstdio>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              file {
  /**
   *  @class cfile cfile.hh "com/centreon/broker/file/cfile.hh"
   *  @brief Wrapper for C-style FILE streams.
   *
   *  Wrap calls that work on C FILE streams.
   */
  class                cfile {
  public:
                       cfile();
                       ~cfile() throw ();
    void               close() throw ();
    void               open(char const* path, char const* mode);
    unsigned long      read(void* buffer, unsigned long max_size);
    void               seek(long offset, int whence = SEEK_SET);
    long               tell();
    unsigned long      write(void const* buffer, unsigned long size);

  private:
                       cfile(cfile const& right);
    cfile&             operator=(cfile const& right);

    FILE*              _stream;
  };
}

CCB_END()

#endif // !CCB_FILE_CFILE_HH
