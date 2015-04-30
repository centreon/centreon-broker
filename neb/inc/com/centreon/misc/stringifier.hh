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

#ifndef CC_MISC_STRINGIFIER_HH
#  define CC_MISC_STRINGIFIER_HH

#  include <string>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace        misc {
  /**
   *  @class stringifier stringifier.hh "com/centreon/misc/stringifier.hh"
   *  @brief Provide method to converting data to string.
   *
   *  Stringifier is a simple way to convert different data type to
   *  C-String (null pointer terminate).
   */
  class          stringifier {
  public:
                 stringifier(char const* buffer = NULL) throw ();
                 stringifier(stringifier const& right);
    virtual      ~stringifier() throw ();
    stringifier& operator=(stringifier const& right);
    stringifier& operator<<(bool b) throw ();
    stringifier& operator<<(char const* str) throw ();
    stringifier& operator<<(char c) throw ();
    stringifier& operator<<(double d) throw ();
    stringifier& operator<<(int i) throw ();
    stringifier& operator<<(long long ll) throw ();
    stringifier& operator<<(long l) throw ();
    stringifier& operator<<(std::string const& str) throw ();
    stringifier& operator<<(stringifier const& str) throw ();
    stringifier& operator<<(unsigned int u) throw ();
    stringifier& operator<<(unsigned long long ull) throw ();
    stringifier& operator<<(unsigned long ul) throw ();
    stringifier& operator<<(void const* p) throw ();
    stringifier& append(char const* str, unsigned int size) throw ();
    char const*  data() const throw ();
    int          precision() const throw ();
    void         precision(int val) throw ();
    void         reset() throw ();
    unsigned int size() const throw ();

  private:
    template     <typename T>
    stringifier& _insert(char const* format, T t) throw ();
    template     <typename T>
    stringifier& _insert(
                   char const* format,
                   unsigned int limit,
                   T t) throw ();
    stringifier& _internal_copy(stringifier const& right);
    bool         _realloc(unsigned int new_size) throw ();

    char*         _buffer;
    unsigned int  _current;
    int           _precision;
    unsigned int  _size;
    static unsigned int const
                  _static_buffer_size = 1024;
    char          _static_buffer[_static_buffer_size];
  };
}

CC_END()

#endif // !CC_MISC_STRINGIFIER_HH
