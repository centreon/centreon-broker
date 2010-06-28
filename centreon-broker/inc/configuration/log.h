/*
**  Copyright 2009 MERETHIS
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

#ifndef CONFIGURATION_LOG_H_
# define CONFIGURATION_LOG_H_

# include <string>

namespace              Configuration
{
  /**
   *  \class Log log.h "configuration/log.h"
   *  \brief Holds configuration parameters of a logging object.
   *
   *  The Log class holds configuration parameters for a logging object. This
   *  can be some file, stdout, stderr or syslog. Users should check the type
   *  before fetching parameters.
   */
  class                Log
  {
   public:
    enum               Type
    {
      UNKNOWN = 1,
      FILE,
      STDERR,
      STDOUT,
      SYSLOG
    };
    int                facility;
    std::string        file;
    unsigned int       flags;
    std::string        name;
    Type               type;
                       Log();
                       Log(const Log& l);
                       ~Log();
    Log&               operator=(const Log& l);
    bool               operator==(const Log& l) const;
    bool               operator!=(const Log& l) const;
    bool               operator<(const Log& l) const;
  };
}

#endif /* !CONFIGURATION_LOG_H_ */
