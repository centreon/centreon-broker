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

#ifndef LOGGING_OSTREAM_HH_
# define LOGGING_OSTREAM_HH_

# include <ostream>
# include "logging/backend.hh"

namespace         logging
{
  /**
   *  @class ostream ostream.hh "logging/ostream.hh"
   *  @brief Log messages to standard output streams.
   *
   *  Log messages to standard output stream objects, like std::cout or
   *  std::cerr.
   *
   *  @see logging::file
   */
  class           ostream : public backend
  {
   protected:
    std::ostream* _os;

   public:
                  ostream();
                  ostream(ostream const& o);
                  ostream(std::ostream& os);
    virtual       ~ostream();
    ostream&      operator=(ostream const& o);
    ostream&      operator=(std::ostream& os);
    void          log_msg(char const* msg,
                    unsigned int len,
                    type log_type,
                    level l) throw ();
  };
}

#endif /* !LOGGING_OSTREAM_HH_ */
