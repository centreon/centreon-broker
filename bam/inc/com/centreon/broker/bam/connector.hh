/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_CONNECTOR_HH
#  define CCB_BAM_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           bam {
  /**
   *  @class connector connector.hh "com/centreon/broker/bam/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send perfdata in a Centreon bam database.
   */
  class             connector : public io::endpoint {
  public:
    enum stream_type {
      bam_type = 1,
      bam_bi_type
    };

                    connector();
                    connector(connector const& other);
                    ~connector();
    connector&      operator=(connector const& other);
    void            connect_to(
                      stream_type type,
                      database_config const& db_cfg);
    misc::shared_ptr<io::stream>
                    open();

   private:
    void            _internal_copy(connector const& other);

    database_config _db_cfg;
    stream_type     _type;
  };
}

CCB_END()

#endif // !CCB_BAM_CONNECTOR_HH
