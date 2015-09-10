/*
** Copyright 2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
