/*
** Copyright 2013 Centreon
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
                    bool is_in,
                    bool is_out,
                    bool negociate,
                    QString const& extensions,
                    time_t timeout,
                    bool coarse = false);
                  connector(connector const& right);
                  ~connector();
    connector&    operator=(connector const& right);
    io::endpoint* clone() const;
    void          close();
    misc::shared_ptr<io::stream>
                  open();
    misc::shared_ptr<io::stream>
                  open(QString const& id);

  private:
    misc::shared_ptr<io::stream>
                  _open(misc::shared_ptr<io::stream> stream);

    bool          _coarse;
    QString       _extensions;
    bool          _is_in;
    bool          _is_out;
    bool          _negociate;
    time_t        _timeout;
  };
}

CCB_END()

#endif // !CCB_BBDO_CONNECTOR_HH
