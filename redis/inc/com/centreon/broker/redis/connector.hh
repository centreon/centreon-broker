/*
** Copyright 2018 Centreon
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

#ifndef CCB_REDIS_CONNECTOR_HH
#  define CCB_REDIS_CONNECTOR_HH

#  include "com/centreon/broker/io/endpoint.hh"

CCB_BEGIN()

namespace                        redis {
  /**
   *  @class connector connector.hh "com/centreon/broker/redis/connector.hh"
   *  @brief Connect to a Redis database.
   *
   *  Send events to a Redis database.
   */
  class                          connector : public io::endpoint {
  public:
                                 connector();
                                 connector(connector const& other);
                                 ~connector();
    connector&                   operator=(connector const& other);
    void                         connect_to(
                                   std::string const& address,
                                   unsigned short port,
                                   std::string const& user,
                                   std::string const& password);
    misc::shared_ptr<io::stream> open();

  private:
    std::string                  _address;
    unsigned short               _port;
    std::string                  _user;
    std::string                  _password;
  };
}

CCB_END()

#endif // !CCB_REDIS_CONNECTOR_HH
