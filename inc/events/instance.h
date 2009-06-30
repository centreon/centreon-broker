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

#ifndef INSTANCE_H_
# define INSTANCE_H_

# include <string>
# include "events/event.h"

namespace       CentreonBroker
{
  namespace       Events
  {
    class         Instance : public Event
    {
     private:
      enum        String
      {
	ADDRESS = 0,
	DESCRIPTION,
	NAME,
	STRING_NB
      };
      std::string strings_[STRING_NB];
      void        InternalCopy(const Instance& inst);

     public:
                  Instance();
		  Instance(const Instance& inst);
		  ~Instance();
      Instance&   operator=(const Instance& inst);
      int         GetType() const throw ();
    };
  }
}

#endif /* !INSTANCE_H_ */
