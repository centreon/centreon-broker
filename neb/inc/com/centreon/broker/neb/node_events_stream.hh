/*
** Copyright 2015 Merethis
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

#ifndef CCB_NEB_NODE_EVENTS_STREAM_HH
#  define CCB_NEB_NODE_EVENTS_STREAM_HH

#  include <QString>
#  include <QHash>
#  include <QPair>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/command_file/external_command.hh"
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/downtime_scheduler.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/node_id.hh"
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/time/timeperiod.hh"
#  include "com/centreon/broker/neb/node_cache.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class node_events_stream node_events_stream.hh "com/centreon/broker/neb/node_events_stream.hh"
   *  @brief Node events stream.
   *
   *  Manage node events: downtime, acks, etc.
   */
  class          node_events_stream : public io::stream {
  public:
                 node_events_stream(
                   misc::shared_ptr<persistent_cache> cache,
                   bool with_timeperiods,
                   database_config const& conf);
                 ~node_events_stream();
    void         process(bool in = false, bool out = true);
    void         read(misc::shared_ptr<io::data>& d);
    void         update();
    unsigned int write(misc::shared_ptr<io::data> const& d);
    void         parse_command(
                   command_file::external_command const& exc,
                   io::stream& stream);
    void         set_timeperiods(
                   QHash<QString, time::timeperiod::ptr> const& tps);

  private:
                 node_events_stream(node_events_stream const& other);
    node_events_stream&
                 operator=(node_events_stream const& other);

    misc::shared_ptr<persistent_cache>
                 _cache;
    database_config
                 _conf;
    bool         _process_out;

    // Timeperiods.
    QHash<QString, time::timeperiod::ptr>
                 _timeperiods;
    bool         _with_timeperiods;

    // Host/Service caches.
    node_cache   _node_cache;

    void         _process_host_status(
                   neb::host_status const& hst);
    void         _process_service_status(
                   neb::service_status const& sst);
    void          _update_downtime(
                   neb::downtime const& dwn);
    void         _remove_expired_acknowledgement(
                   node_id node,
                   short prev_state,
                   short state);
    void         _trigger_floating_downtime(
                   node_id node,
                   timestamp check_time,
                   short state);

    // Acks and downtimes caches.
    QHash<node_id, neb::acknowledgement>
                 _acknowledgements;
    QHash<unsigned int, neb::downtime>
                 _downtimes;
    QMultiHash<node_id, unsigned int>
                 _downtime_id_by_nodes;
    unsigned int _actual_downtime_id;
    downtime_scheduler
                 _downtime_scheduler;
    QHash<unsigned int, neb::downtime>
                 _recurring_downtimes;

    enum         ack_type {
                 ack_host = 0,
                 ack_service
    };

    enum         down_type {
                 down_service = 1,
                 down_host = 2,
                 down_host_service = 3
    };
    void         _parse_ack(
                   ack_type type,
                   timestamp t,
                   const char* args,
                   io::stream& stream);
    void         _parse_remove_ack(
                   ack_type type,
                   timestamp t,
                   const char* args,
                   io::stream& stream);
    void         _parse_downtime(
                   down_type type,
                   timestamp t,
                   const char* args,
                   io::stream& stream);
    void         _parse_remove_downtime(
                   down_type type,
                   timestamp t,
                   const char* args,
                   io::stream& stream);
    void          _schedule_downtime(
                    downtime const& dwn);
    void          _spawn_recurring_downtime(
                    timestamp when,
                    downtime const& dwn);

    void         _load_timeperiods();
    void         _check_downtime_timeperiod_consistency();
    void         _load_cache();
    void         _process_loaded_event(misc::shared_ptr<io::data> const& d);
    void         _save_cache();
  };
}

CCB_END()

#endif // !CCB_NEB_NODE_EVENTS_STREAM_HH
