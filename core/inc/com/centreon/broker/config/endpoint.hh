/*
** Copyright 2009-2012 Centreon
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

#ifndef CCB_CONFIG_ENDPOINT_HH
#  define CCB_CONFIG_ENDPOINT_HH

#  include <ctime>
#  include <QMap>
#  include <QString>
#  include <set>
#  include <QDomElement>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    config {
  /**
   *  @class endpoint endpoint.hh "com/centreon/broker/config/endpoint.hh"
   *  @brief Hold configuration of an endpoint.
   *
   *  An endpoint is an external source or destination for events.
   *  This can either be an XML stream, a database, a file, ...
   *  This class holds the configuration of an endpoint.
   */
  class                      endpoint {
  public:
                             endpoint();
                             endpoint(endpoint const& e);
                             ~endpoint();
    endpoint&                operator=(endpoint const& e);
    bool                     operator==(endpoint const& e) const;
    bool                     operator!=(endpoint const& e) const;
    bool                     operator<(endpoint const& e) const;

    time_t                   buffering_timeout;
    QString                  failover;
    QString                  name;
    QMap<QString, QString>   params;
    time_t                   read_timeout;
    time_t                   retry_interval;
    std::set<std::string>    filters;
    QString                  type;
    QDomElement              cfg;

  private:
    void                     _internal_copy(endpoint const& e);
  };
}

CCB_END()

#endif /* !CCB_CONFIG_ENDPOINT_HH */
