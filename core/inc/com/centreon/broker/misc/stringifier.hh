/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_MISC_STRINGIFIER_HH_
#  define CCB_MISC_STRINGIFIER_HH_

#  include <QString>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                     misc {
  /**
   *  @class stringifier stringifier.hh "com/centreon/broker/misc/stringifier.hh"
   *  @brief Help converting various data types to string.
   *
   *  stringifier accepts various data types on input and convert them
   *  into a C-string (nul-terminated). Whereas stringifier has a
   *  maximum number of characters it can store, it's main advantage is
   *  that it is totally safe to use it (no exception will ever be
   *  thrown) and therefore make it suitable for exception messages or
   *  log messages.
   */
  class                       stringifier {
   public:
    static unsigned int const max_len = 6000;

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
    stringifier&              operator<<(long l) throw ();
    stringifier&              operator<<(long long ll) throw ();
    stringifier&              operator<<(QString const& q) throw ();
    stringifier&              operator<<(std::string const& s) throw ();
    stringifier&              operator<<(unsigned int i) throw ();
    stringifier&              operator<<(unsigned long l) throw ();
    stringifier&              operator<<(unsigned long long l) throw ();
    stringifier&              operator<<(char const* str) throw ();
    stringifier&              operator<<(void const* p) throw ();
    char const*               data() const throw ();
    void                      reset() throw ();
  };
}

CCB_END()

#endif /* !CCB_MISC_STRINGIFIER_HH_ */
