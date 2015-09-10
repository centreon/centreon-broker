/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_TCP_STREAM_HH
#  define CCB_TCP_STREAM_HH

#  include <memory>
#  include <QTcpSocket>
#  include <QMutex>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/tcp/socket_parent.hh"

CCB_BEGIN()

namespace        tcp {
  // Forward declaration.
  class          acceptor;

  /**
   *  @class stream stream.hh "com/centreon/broker/tcp/stream.hh"
   *  @brief TCP stream.
   *
   *  TCP stream.
   */
  class          stream : public io::stream {
  public:
                 stream(QTcpSocket* sock, std::string const& name);
                 stream(int socket_descriptor);
                 ~stream();
    void         process(bool in = false, bool out = true);
    void         read(misc::shared_ptr<io::data>& d);
    std::string const&
                 get_name() const throw();
    void         set_parent(socket_parent* parent);
    void         set_read_timeout(int secs);
    void         set_write_timeout(int secs);
    unsigned int write(misc::shared_ptr<io::data> const& d);
    void         close();

  private:
                 stream(stream const& other);
    stream&      operator=(stream const& other);
    void         _initialize_socket();
    void         _set_socket_options();
    void         _stop();

    bool         _process_in;
    bool         _process_out;
    QMutex       _mutex;
    std::string  _name;
    socket_parent*
                 _parent;
    int          _read_timeout;
    std::auto_ptr<QTcpSocket>
                 _socket;
    int          _socket_descriptor;
    int          _write_timeout;
  };
}

CCB_END()

#endif // !CCB_TCP_STREAM_HH
