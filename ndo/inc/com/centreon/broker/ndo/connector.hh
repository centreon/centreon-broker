/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_NDO_CONNECTOR_HH
#  define CCB_NDO_CONNECTOR_HH

#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         ndo {
  /**
   *  @class connector connector.hh "com/centreon/broker/ndo/connector.hh"
   *  @brief NDO connector.
   *
   *  Initiate NDO connections.
   */
  class           connector : public io::endpoint {
  public:
                  connector(bool is_in, bool is_out);
                  connector(connector const& c);
                  ~connector();
    connector&    operator=(connector const& c);
    io::endpoint* clone() const;
    void          close();
    misc::shared_ptr<io::stream>
                  open();
    misc::shared_ptr<io::stream>
                  open(QString const& id);

  private:
    misc::shared_ptr<io::stream>
                  _open(misc::shared_ptr<io::stream> stream);

    bool          _is_in;
    bool          _is_out;
  };
}

CCB_END()

#endif // !CCB_NDO_CONNECTOR_HH
