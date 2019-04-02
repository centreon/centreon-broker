/*
** Copyright 2017-2018 Centreon
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

#ifndef CCB_SIMU_CONNECTOR_HH
#  define CCB_SIMU_CONNECTOR_HH

#  include <QMap>
#  include <QVariant>
#  include "com/centreon/broker/io/endpoint.hh"

CCB_BEGIN()

namespace                        simu {
  /**
   *  @class connector connector.hh "com/centreon/broker/simu/connector.hh"
   *  @brief Connect to a lua interpreter.
   *
   *  Send events to a lua interpreter.
   */
  class                          connector : public io::endpoint {
  public:
                                 connector();
                                 connector(connector const& other);
                                 ~connector();
    connector&                   operator=(connector const& other);
    void                         connect_to(
                                   std::string const& lua_script,
                                   QMap<QString, QVariant> const& cfg_params,
                                   misc::shared_ptr<persistent_cache> const& cache);
    misc::shared_ptr<io::stream> open();

  private:
    std::string                  _lua_script;
    QMap<QString, QVariant>      _conf_params;
    misc::shared_ptr<persistent_cache>
                                 _cache;
  };
}

CCB_END()

#endif // !CCB_SIMU_CONNECTOR_HH
