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

#ifndef IO_IO_H_
# define IO_IO_H_

namespace             CentreonBroker
{
  namespace           IO
  {
    class             Stream
    {
     public:
                      Stream() throw ();
                      Stream(const Stream& stream) throw ();
      virtual         ~Stream();
      Stream&         operator=(const Stream& stream) throw ();
      virtual void    Close() = 0;
      virtual int     Receive(char* buffer, int size) = 0;
      virtual int     Send(const char* buffer, int size) = 0;
    };

    class             Acceptor
    {
     public:
                      Acceptor() throw ();
                      Acceptor(const Acceptor& acceptor) throw ();
      virtual         ~Acceptor();
      Acceptor&       operator=(const Acceptor& acceptor) throw ();
      virtual Stream* Accept() = 0;
      virtual void    Close() = 0;
      virtual void    Listen() = 0;
    };
  }
}

#endif /* !IO_IO_H_ */
