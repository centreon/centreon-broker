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

#ifndef CCB_RRD_CONNECTOR_HH
#  define CCB_RRD_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          rrd {
  /**
   *  @class connector connector.hh "com/centreon/broker/rrd/connector.hh"
   *  @brief RRD connector.
   *
   *  Generate an RRD stream that will write files.
   */
  class            connector : public io::endpoint {
  public:
                   connector();
                   connector(connector const& right);
                   ~connector();
    connector&     operator=(connector const& right);
    misc::shared_ptr<io::stream>
                   open();
    void           set_cache_size(unsigned int cache_size);
    void           set_cached_local(QString const& local_socket);
    void           set_cached_net(unsigned short port) throw ();
    void           set_ignore_update_errors(bool ignore) throw ();
    void           set_metrics_path(QString const& metrics_path);
    void           set_status_path(QString const& status_path);
    void           set_write_metrics(bool write_metrics) throw ();
    void           set_write_status(bool write_status) throw ();

  private:
    void           _internal_copy(connector const& right);
    QString        _real_path_of(QString const& path);

    unsigned int   _cache_size;
    QString        _cached_local;
    unsigned short _cached_port;
    bool           _ignore_update_errors;
    QString        _metrics_path;
    QString        _status_path;
    bool           _write_metrics;
    bool           _write_status;
  };
}

CCB_END()

#endif // !CCB_RRD_CONNECTOR_HH
