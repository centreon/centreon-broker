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

#ifndef IO_NET4_H_
# define IO_NET4_H_

# include "io/io.h"

namespace            CentreonBroker
{
  namespace          IO
  {
    class            Net4Stream : public Stream
    {
      friend class   Net4Acceptor;

     private:
      int            sockfd_;
                     Net4Stream(int sockfd) throw ();
                     Net4Stream(const Net4Stream& n4s);
      Net4Stream&    operator=(const Net4Stream& n4s);

     public:
                     ~Net4Stream();
      void           Close();
      int            Receive(char* buffer, int size);
      int            Send(const char* buffer, int size);
    };

    class            Net4Acceptor : public Acceptor
    {
     private:
      unsigned short port_;
      int            sockfd_;
                     Net4Acceptor(const Net4Acceptor& n4a) throw ();
      Net4Acceptor&  operator=(const Net4Acceptor& n4a) throw ();

     public:
                     Net4Acceptor();
                     ~Net4Acceptor();
      Stream*        Accept();
      void           Close();
      unsigned short GetPort() const throw ();
      void           Listen();
      void           SetPort(unsigned short port);
    };
  }
}

#endif /* !IO_NET4_H_ */
