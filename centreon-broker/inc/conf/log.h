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

#ifndef CONF_LOG_H_
# define CONF_LOG_H_

# include <string>

namespace                CentreonBroker
{
  namespace              Conf
  {
    class                Log
    {
     private:
      enum               Int
      {
	FLAGS = 0,
	INT_NB
      };
      enum               String
      {
	PATH = 0,
	TYPE,
	STRING_NB
      };
      int                ints_[INT_NB];
      std::string        strings_[STRING_NB];
      void               InternalCopy(const Log& l);

     public:
                         Log();
                         Log(const Log& l);
                         ~Log();
      Log&               operator=(const Log& l);
      bool               operator==(const Log& l);
      int                GetFlags() const throw ();
      const std::string& GetPath() const throw ();
      const std::string& GetType() const throw ();
      void               SetFlags(int flags) throw ();
      void               SetPath(const std::string& path);
      void               SetType(const std::string& type);
    };
  }
}

#endif /* !CONF_LOG_H_ */
