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

#ifndef CCB_RRD_OUTPUT_HH
#  define CCB_RRD_OUTPUT_HH

#  include <list>
#  include <memory>
#  include <QHash>
#  include <QString>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/rrd/backend.hh"

CCB_BEGIN()

namespace                    rrd {
  /**
   *  @class output output.hh "com/centreon/broker/rrd/output.hh"
   *  @brief RRD output class.
   *
   *  Write RRD files.
   */
  class                        output : public io::stream {
  public:
    typedef                    QHash<
                                 QString,
                                 std::list<misc::shared_ptr<io::data> > >
                               rebuild_cache;

                               output(
                                 QString const& metrics_path,
                                 QString const& status_path,
                                 unsigned int cache_size,
                                 bool ignore_update_errors,
                                 bool write_metrics = true,
                                 bool write_status = true);
                               output(
                                 QString const& metrics_path,
                                 QString const& status_path,
                                 unsigned int cache_size,
                                 bool ignore_update_errors,
                                 QString const& local,
                                 bool write_metrics = true,
                                 bool write_status = true);
                               output(
                                 QString const& metrics_path,
                                 QString const& status_path,
                                 unsigned int cache_size,
                                 bool ignore_update_errors,
                                 unsigned short port,
                                 bool write_metrics = true,
                                 bool write_status = true);
                               ~output();
    bool                       read(
                                 misc::shared_ptr<io::data>& d,
                                 time_t deadline);
    void                       update();
    int                        write(misc::shared_ptr<io::data> const& d);

  private:
                               output(output const& o);
    output&                    operator=(output const& o);

    std::unique_ptr<backend>     _backend;
    bool                       _ignore_update_errors;
    std::string                _metrics_path;
    rebuild_cache              _metrics_rebuild;
    std::string                _status_path;
    rebuild_cache              _status_rebuild;
    bool                       _write_metrics;
    bool                       _write_status;
  };
}

CCB_END()

#endif // !CCB_RRD_OUTPUT_HH
