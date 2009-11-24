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

#ifndef DBUFFER_H_
# define DBUFFER_H_

# include <string>

namespace        CentreonBroker
{
  /**
   *  \class DBuffer dbuffer.h "dbuffer.h"
   *  \brief Dynamic buffer when data can be stored in a binary fashion.
   *
   *  This class represents a buffer whose memory is automatically managed as
   *  data are inserted.
   */
  class          DBuffer
  {
   private:
    std::string  buffer_;

   public:
                 DBuffer();
                 DBuffer(const DBuffer& dbuffer);
                 ~DBuffer();
    DBuffer&     operator=(const DBuffer& dbuffer);
                 operator const void*() const;
    void         Append(bool b);
    void         Append(const char* s);
    void         Append(double d);
    void         Append(int i);
    void         Append(short s);
    void         Append(const std::string& s);
    unsigned int Size() const;
  };
}

#endif /* !DBUFFER_H_ */
