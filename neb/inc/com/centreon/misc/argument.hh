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

#ifndef CC_MISC_ARGUMENT_HH
#  define CC_MISC_ARGUMENT_HH

#  include <string>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace              misc {
  /**
   *  @class argument argument.hh "com/centreon/misc/argument.hh"
   *  @brief Define class argument for get options.
   *
   *  This is an argument class.
   */
  class                argument {
  public:
                       argument(
                         std::string const& long_name = "",
                         char name = '\0',
                         std::string const& description = "",
                         bool has_value = false,
                         bool is_set = false,
                         std::string const& value = "");
                       argument(argument const& right);
                       ~argument() throw ();
    argument&          operator=(argument const& right);
    bool               operator==(argument const& right) const throw ();
    bool               operator!=(argument const& right) const throw ();
    std::string const& get_description() const throw ();
    bool               get_is_set() const throw ();
    bool               get_has_value() const throw ();
    std::string const& get_long_name() const throw ();
    char               get_name() const throw ();
    std::string const& get_value() const throw ();
    void               set_description(std::string const& description);
    void               set_is_set(bool val) throw ();
    void               set_has_value(bool val) throw ();
    void               set_long_name(std::string const& long_name);
    void               set_name(char name);
    void               set_value(std::string const& value);

  private:
    argument&          _internal_copy(argument const& right);

    std::string        _description;
    bool               _is_set;
    bool               _has_value;
    std::string        _long_name;
    char               _name;
    std::string        _value;
  };
}

CC_END()

#endif // !CC_MISC_ARGUMENT_HH
