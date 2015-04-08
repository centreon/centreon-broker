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

#ifndef CC_MISC_COMMAND_LINE_HH
#  define CC_MISC_COMMAND_LINE_HH

#  include <string>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace         misc {
  /**
   *  @class command_line command_line.hh "com/centreon/misc/command_line.hh"
   *  @brief Provide method to split command line arguments into array.
   *
   *  Command line is a simple way to split command line arguments
   *  into array.
   */
  class           command_line {
  public:
                  command_line();
                  command_line(
                    char const* cmdline,
                    unsigned int size = 0);
                  command_line(std::string const& cmdline);
                  command_line(command_line const& right);
                  ~command_line() throw ();
    command_line& operator=(command_line const& right);
    bool          operator==(command_line const& right) const throw ();
    bool          operator!=(command_line const& right) const throw ();
    int           get_argc() const throw ();
    char**        get_argv() const throw ();
    void          parse(char const* cmdline, unsigned int size = 0);
    void          parse(std::string const& cmdline);

  private:
    void          _internal_copy(command_line const& right);
    void          _release();

    int           _argc;
    char**        _argv;
    size_t        _size;
  };
}

CC_END()

#endif // !CC_MISC_COMMAND_LINE_HH
