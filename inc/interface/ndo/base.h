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

#ifndef INTERFACE_NDO_BASE_H_
# define INTERFACE_NDO_BASE_H_

# include "io/text.h"

// Forward declaration
namespace      IO
{ class        Stream; }

namespace      Interface
{
  namespace    NDO
  {
    /**
     *  \class Base base.h "interface/ndo/base.h"
     *  \brief Base class of NDO protocol classes.
     *
     *  Holds the stream object so that destruction will only occur once.
     */
    class      Base
    {
     private:
               Base(const Base& base);
      Base&    operator=(const Base& base);

     protected:
      IO::Text stream_;

     public:
               Base(IO::Stream* stream);
      virtual  ~Base();
    };
  }
}

#endif /* !INTERFACE_NDO_BASE_H_ */
