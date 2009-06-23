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

#ifndef PROTOCOL_SOCKET_H_
# define PROTOCOL_SOCKET_H_

# include <boost/asio.hpp>
# ifdef USE_TLS
#  include <boost/asio/ssl.hpp>
# endif /* USE_TLS */
# include <cstring>

namespace           CentreonBroker
{
  /**
   *  The ProtocolSocket class will buffer input from the socket and make it
   *  available a line at a time.
   */
  class             ProtocolSocket
  {
   private:
    char            buffer_[1024];
    unsigned long   bytes_processed_;
    size_t          discard_;
    time_t          last_checkin_time_;
    size_t          length_;
    unsigned long   lines_processed_;
                    ProtocolSocket(const ProtocolSocket& ps) throw ();
    ProtocolSocket& operator=(const ProtocolSocket& ps) throw ();

   protected:
    template        <typename SocketT>
    char*           GetLine(SocketT& socket);

   public:
                    ProtocolSocket() throw ();
    virtual         ~ProtocolSocket();
    unsigned long   GetBytesProcessed() const;
    time_t          GetLastCheckinTime() const;
    virtual char*   GetLine() = 0;
    unsigned long   GetLinesProcessed() const;
  };

  /**
   *  This class is a ProtocolSocket that uses a normal boost socket (as
   *  opposed to TLS socket.
   */
  class                     StandardProtocolSocket : public ProtocolSocket
  {
   private:
    std::auto_ptr<boost::asio::ip::tcp::socket> socket_;
                            StandardProtocolSocket(
                              const StandardProtocolSocket& sps);
    StandardProtocolSocket& operator=(const StandardProtocolSocket& sps);

   public:
                            StandardProtocolSocket(
                              boost::asio::ip::tcp::socket* socket);
			    ~StandardProtocolSocket();
    char*                   GetLine();
  };

# ifdef USE_TLS
  /**
   *  This class is a ProtocolSocket that uses a TLS socket.
   */
  class                TlsProtocolSocket : public ProtocolSocket
  {
   private:
    std::auto_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket> >
      socket_;
                       TlsProtocolSocket(const TlsProtocolSocket& tps);
    TlsProtocolSocket& operator=(const TlsProtocolSocket& tps);

   public:
                       TlsProtocolSocket(
                         boost::asio::ssl::stream<
                           boost::asio::ip::tcp::socket>*);
                       ~TlsProtocolSocket();
    char*              GetLine();
  };
# endif /* USE_TLS */
}

#endif /* !PROTOCOL_SOCKET_H_ */
