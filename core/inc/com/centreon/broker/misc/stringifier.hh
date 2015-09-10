/*
** Copyright 2009-2011 Centreon
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
