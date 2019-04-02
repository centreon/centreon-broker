/*
** Copyright 2013,2015,2017 Centreon
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

#ifndef CCB_BBDO_ACCEPTOR_HH
#  define CCB_BBDO_ACCEPTOR_HH

#  include <ctime>
#  include <list>
#  include <QMutex>
#  include <QString>
#  include <QThread>
#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace         processing {
  class           thread;
}

namespace         bbdo {
  // Forward declaration.
  class           stream;

  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/bbdo/acceptor.hh"
   *  @brief BBDO acceptor.
   *
   *  Accept incoming BBDO connections.
   */
  class           acceptor : public io::endpoint {
  public:
                  acceptor(
                    std::string const& name,
                    bool negotiate,
                    QString const& extensions,
                    time_t timeout,
                    bool one_peer_retention_mode = false,
                    bool coarse = false,
                    unsigned int ack_limit = 1000);
                  acceptor(acceptor const& other);
                  ~acceptor();
    acceptor&     operator=(acceptor const& other);
    std::shared_ptr<io::stream>
                  open();
    void          stats(io::properties& tree);

  private:
    unsigned int  _negotiate_features(
                     std::shared_ptr<io::stream> stream,
                     std::shared_ptr<bbdo::stream> my_bbdo);
    void          _open(std::shared_ptr<io::stream> stream);

    bool          _coarse;
    QString       _extensions;
    std::string   _name;
    bool          _negotiate;
    bool          _one_peer_retention_mode;
    time_t        _timeout;
    unsigned int  _ack_limit;
  };
}

CCB_END()

#endif // !CCB_BBDO_ACCEPTOR_HH
