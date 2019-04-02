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

#ifndef CCB_BBDO_CONNECTOR_HH
#  define CCB_BBDO_CONNECTOR_HH

#  include <ctime>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         bbdo {
  /**
   *  @class connector connector.hh "com/centreon/broker/bbdo/connector.hh"
   *  @brief BBDO connector.
   *
   *  Initiate direct BBDO protocol connections.
   */
  class           connector : public io::endpoint {
  public:
                  connector(
                    bool negotiate,
                    QString const& extensions,
                    time_t timeout,
                    bool coarse = false,
                    unsigned int ack_limit = 1000);
                  connector(connector const& other);
                  ~connector();
    connector&    operator=(connector const& other);
    std::shared_ptr<io::stream>
                  open();

  private:
    std::shared_ptr<io::stream>
                  _open(std::shared_ptr<io::stream> stream);

    bool          _coarse;
    QString       _extensions;
    bool          _negotiate;
    time_t        _timeout;
    unsigned int  _ack_limit;
  };
}

CCB_END()

#endif // !CCB_BBDO_CONNECTOR_HH
