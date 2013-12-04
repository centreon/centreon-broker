/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
    void                       process(
                                 bool in = false,
                                 bool out = true);
    void                       read(misc::shared_ptr<io::data>& d);
    void                       update();
    unsigned int               write(
                                 misc::shared_ptr<io::data> const& d);

  private:
                               output(output const& o);
    output&                    operator=(output const& o);

    std::auto_ptr<backend>     _backend;
    bool                       _ignore_update_errors;
    std::string                _metrics_path;
    rebuild_cache              _metrics_rebuild;
    bool                       _process_out;
    std::string                _status_path;
    rebuild_cache              _status_rebuild;
    bool                       _write_metrics;
    bool                       _write_status;
  };
}

CCB_END()

#endif // !CCB_RRD_OUTPUT_HH
