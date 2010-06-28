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

#ifndef LOGGING_FILE_HH_
# define LOGGING_FILE_HH_

# include <fstream>
# include "logging/ostream.hh"

namespace         logging
{
  /**
   *  @class file file.hh "logging/file.hh"
   *  @brief Log messages to a file.
   *
   *  Log messages to a file.
   */
  class           file : public ostream
  {
   private:
    std::ofstream _ofs;
                  file(file const& f);
    file&         operator=(file const& f);

   public:
                  file();
                  file(char const* filename);
                  ~file();
    void          open(char const* filename);
  };
}

#endif /* !LOGGING_FILE_HH_ */
