/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_MONITORING_STREAM_HH
#  define CCB_BAM_MONITORING_STREAM_HH

#  include <string>
#  include "com/centreon/broker/bam/ba_svc_mapping.hh"
#  include "com/centreon/broker/bam/configuration/applier/state.hh"
#  include "com/centreon/broker/database.hh"
#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class monitoring_stream monitoring_stream.hh "com/centreon/broker/bam/monitoring_stream.hh"
   *  @brief bam monitoring_stream.
   *
   *  Handle perfdata and insert proper informations in index_data and
   *  metrics table of a centbam DB.
   */
  class            monitoring_stream : public io::stream {
  public:
                   monitoring_stream(
                     std::string const& db_type,
                     std::string const& db_host,
                     unsigned short db_port,
                     std::string const& db_user,
                     std::string const& db_password,
                     std::string const& db_name,
                     std::string const& ext_cmd_file,
                     unsigned int queries_per_transaction,
                     bool check_replication = true);
                   ~monitoring_stream();
    void           initialize();
    void           process(bool in = false, bool out = true);
    void           read(misc::shared_ptr<io::data>& d);
    void           statistics(io::properties& tree) const;
    void           update();
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
                   monitoring_stream(monitoring_stream const& other);
    monitoring_stream&
                   operator=(monitoring_stream const& other);
    void           _check_replication();
    void           _prepare();
    void           _rebuild();
    void           _update_status(std::string const& status);
    void           _write_external_command(std::string const& cmd);

    configuration::applier::state
                   _applier;
    ba_svc_mapping _ba_mapping;
    std::string    _ext_cmd_file;
    bool           _process_out;
    std::string    _status;
    mutable QMutex _statusm;
    database       _db;
    database_query _ba_update;
    database_query _bool_exp_update;
    database_query _kpi_update;
    database_query _meta_service_update;
    int            _pending_events;
  };
}

CCB_END()

#endif // !CCB_BAM_MONITORING_STREAM_HH
