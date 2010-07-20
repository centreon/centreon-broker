/*
**  Copyright 2010 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef MISC_STRINGIFIER_HH_
# define MISC_STRINGIFIER_HH_

namespace                     misc
{
  /**
   *  @class safe_buffer safe_buffer.hh "misc/safe_buffer.hh"
   *  @brief Help converting various data types to string.
   *
   *  stringifier accepts various data types on input and convert them into a
   *  C-string (nul-terminated). Whereas stringifier has a maximum number of
   *  characters it can store, it's main advantage is that it is totally safe
   *  to use it (no exception will ever be thrown) and therefore make it
   *  suitable for exception messages or log messages.
   */
  class                       stringifier
  {
   public:
    static unsigned int const max_len = 1000;

   private:
    template                  <typename T>
    stringifier&              _numeric_conversion(char const* format, T t);

   protected:
    char                      _buffer[max_len + 1];
    unsigned int              _current;

   public:
                              stringifier() throw ();
                              stringifier(stringifier const& s) throw ();
    virtual                   ~stringifier();
    stringifier&              operator=(stringifier const& s);
    stringifier&              operator<<(bool b) throw ();
    stringifier&              operator<<(double d) throw ();
    stringifier&              operator<<(int i) throw ();
    stringifier&              operator<<(unsigned int i) throw ();
    stringifier&              operator<<(char const* str) throw ();
    void                      reset() throw ();
  };
}

#endif /* !MISC_SAFE_BUFFER_HH_ */
