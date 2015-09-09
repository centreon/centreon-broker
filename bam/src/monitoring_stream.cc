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

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <QMutexLocker>
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/bam/monitoring_stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/bam/event_cache_visitor.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg           Database configuration.
 */
monitoring_stream::monitoring_stream(database_config const& db_cfg)
  : _db(db_cfg),
    _ba_update(_db),
    _kpi_update(_db),
    _meta_service_update(_db),
    _pending_events(0) {
  // Prepare queries.
  _prepare();

  // Read configuration from DB.
  configuration::state s;
  {
    configuration::reader r(_db);
    r.read(s);
  }

  // Apply configuration.
  _applier.apply(s);

  // Check if we need to rebuild something.
  _rebuild();
}

/**
 *  Destructor.
 */
monitoring_stream::~monitoring_stream() {}

/**
 *  Generate default state.
 */
void monitoring_stream::initialize() {
  multiplexing::publisher pblshr;
  event_cache_visitor ev_cache;
  _applier.visit(&ev_cache);
  ev_cache.commit_to(pblshr);
  return ;
}

/**
 *  Read from the datbase.
 *  Get the next available bam event.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool monitoring_stream::read(
                          misc::shared_ptr<io::data>& d,
                          time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from BAM monitoring stream");
  return (true);
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void monitoring_stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
  return ;
}

/**
 *  Rebuild index and metrics cache.
 */
void monitoring_stream::update() {
  try {
    configuration::state s;
    {
      configuration::reader r(_db);
      r.read(s);
    }
    _applier.apply(s);
    _rebuild();
    initialize();
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM: could not process configuration update: " << e.what());
  }
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int monitoring_stream::write(misc::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;

  if (!data.isNull()) {
    // Process service status events.
    if ((data->type() == neb::service_status::static_type())
        || (data->type() == neb::service::static_type())) {
      misc::shared_ptr<neb::service_status>
        ss(data.staticCast<neb::service_status>());
      logging::debug(logging::low)
        << "BAM: processing service status (host "
        << ss->host_id << ", service " << ss->service_id
        << ", hard state " << ss->last_hard_state << ", current state "
        << ss->current_state << ")";
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(ss, &ev_cache);
      ev_cache.commit_to(pblshr);
    }
    else if (data->type() == neb::acknowledgement::static_type()) {
      misc::shared_ptr<neb::acknowledgement>
	ack(data.staticCast<neb::acknowledgement>());
      logging::debug(logging::low)
	<< "BAM: processing acknowledgement (host "
	<< ack->host_id << ", service " << ack->service_id << ")";
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(ack, &ev_cache);
      ev_cache.commit_to(pblshr);
    }
    else if (data->type() == neb::downtime::static_type()) {
      misc::shared_ptr<neb::downtime>
	dt(data.staticCast<neb::downtime>());
      logging::debug(logging::low)
	<< "BAM: processing downtime (host " << dt->host_id
	<< ", service " << dt->service_id << ")";
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(dt, &ev_cache);
      ev_cache.commit_to(pblshr);
    }
    else if (data->type() == storage::metric::static_type()) {
      misc::shared_ptr<storage::metric>
        m(data.staticCast<storage::metric>());
      logging::debug(logging::low)
        << "BAM: processing metric (id " << m->metric_id << ", time "
        << m->ctime << ", value " << m->value << ")";
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_metric().update(m, &ev_cache);
      ev_cache.commit_to(pblshr);
    }
    else if (data->type() == bam::ba_status::static_type()) {
      ba_status* status(static_cast<ba_status*>(data.data()));
      logging::debug(logging::low) << "BAM: processing BA status (id "
        << status->ba_id << ", level " << status->level_nominal
        << ", acknowledgement " << status->level_acknowledgement
        << ", downtime " << status->level_downtime << ")";
      _ba_update.bind_value(":level_nominal", status->level_nominal);
      _ba_update.bind_value(
                   ":level_acknowledgement",
                   status->level_acknowledgement);
      _ba_update.bind_value(":level_downtime", status->level_downtime);
      _ba_update.bind_value(":ba_id", status->ba_id);
      _ba_update.bind_value(
        ":last_state_change",
        (((status->last_state_change == (time_t)-1)
          || (status->last_state_change == 0))
         ? QVariant(QVariant::LongLong)
         : QVariant(static_cast<qlonglong>(status->last_state_change.get_time_t()))));
      _ba_update.bind_value(":state", status->state);
      _ba_update.bind_value(":in_downtime", status->in_downtime);
      try { _ba_update.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg() << "BAM: could not update BA "
               << status->ba_id << ": " << e.what());
      }
    }
    else if (data->type() == bam::kpi_status::static_type()) {
      kpi_status* status(static_cast<kpi_status*>(data.data()));
      logging::debug(logging::low) << "BAM: processing KPI status (id "
        << status->kpi_id << ", level " << status->level_nominal_hard
        << ", acknowledgement " << status->level_acknowledgement_hard
        << ", downtime " << status->level_downtime_hard << ")";
      _kpi_update.bind_value(
                    ":level_nominal",
                    status->level_nominal_hard);
      _kpi_update.bind_value(
                    ":level_acknowledgement",
                    status->level_acknowledgement_hard);
      _kpi_update.bind_value(
                    ":level_downtime",
                    status->level_downtime_hard);
      _kpi_update.bind_value(":state", status->state_hard);
      _kpi_update.bind_value(":state_type", 1 + 1);
      _kpi_update.bind_value(":kpi_id", status->kpi_id);
      _kpi_update.bind_value(
        ":last_state_change",
        (((status->last_state_change == (time_t)-1)
          || (status->last_state_change == 0))
         ? QVariant(QVariant::LongLong)
         : QVariant(static_cast<qlonglong>(status->last_state_change.get_time_t()))));
      _kpi_update.bind_value(":last_impact", status->last_impact);
      _kpi_update.bind_value(":valid", status->valid);
      try { _kpi_update.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg() << "BAM: could not update KPI "
               << status->kpi_id << ": " << e.what());
      }
    }
    else if (data->type() == bam::meta_service_status::static_type()) {
      meta_service_status* status(static_cast<meta_service_status*>(data.data()));
      logging::debug(logging::low)
        << "BAM: processing meta-service status (id "
        << status->meta_service_id << ", value " << status->value
        << ")";
      _meta_service_update.bind_value(
                              ":meta_service_id",
                              status->meta_service_id);
      _meta_service_update.bind_value(":value", status->value);
      try { _meta_service_update.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM: could not update meta-service "
               << status->meta_service_id << ": " << e.what());
      }
    }
  }
  else
    _db.commit();

  // Event acknowledgement.
  if (_db.committed()) {
    int retval(_pending_events);
    _db.clear_committed_flag();
    _pending_events = 0;
    return (retval);
  }
  else
    return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Prepare queries.
 */
void monitoring_stream::_prepare() {
  // BA status.
  {
    std::string query;
    query = "UPDATE cfg_bam"
            "  SET current_level=:level_nominal,"
            "      acknowledged=:level_acknowledgement,"
            "      downtime=:level_downtime,"
            "      last_state_change=:last_state_change,"
            "      in_downtime=:in_downtime,"
            "      current_status=:state"
            "  WHERE ba_id=:ba_id";
    try {
      _ba_update.prepare(query);
    }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM: could not prepare BA update query: "
             << e.what());
    }
  }

  // KPI status.
  {
    std::string query;
    query = "UPDATE cfg_bam_kpi"
            "  SET acknowledged=:level_acknowledgement,"
            "      current_status=:state,"
            "      downtime=:level_downtime, last_level=:level_nominal,"
            "      state_type=:state_type,"
            "      last_state_change=:last_state_change,"
            "      last_impact=:last_impact, valid=:valid"
            "  WHERE kpi_id=:kpi_id";
    _kpi_update.prepare(
                  query,
                  "BAM: could not prepare KPI update query");
  }

  // Meta-service status.
  {
    std::string query;
    query = "UPDATE cfg_meta_services"
            "  SET value=:value"
            "  WHERE meta_id=:meta_service_id";
    _meta_service_update.prepare(
      query,
      "BAM: could not prepare meta-service update query");
  }

  return ;
}

/**
 *  Rebuilds BA durations/availibities from BA events.
 */
void monitoring_stream::_rebuild() {
  // Get the list of the BAs that should be rebuild.
  std::vector<unsigned int> bas_to_rebuild;
  {
    std::string query = "SELECT ba_id"
                        "  FROM cfg_bam"
                        "  WHERE must_be_rebuild='1'";
    database_query q(_db);
    q.run_query(
        query,
        "BAM: could not select the list of BAs to rebuild");
    while (q.next())
      bas_to_rebuild.push_back(q.value(0).toUInt());
  }

  // Nothing to rebuild.
  if (bas_to_rebuild.empty())
    return ;

  logging::debug(logging::medium)
    << "BAM: rebuild asked, sending the rebuild signal";

  misc::shared_ptr<rebuild> r(new rebuild);
  {
    std::ostringstream oss;
    for (std::vector<unsigned int>::const_iterator
           it(bas_to_rebuild.begin()),
           end(bas_to_rebuild.end());
         it != end;
         ++it)
      oss << *it << ", ";
    r->bas_to_rebuild = oss.str().c_str();
    r->bas_to_rebuild.resize(r->bas_to_rebuild.size() - 2);
  }
  std::auto_ptr<io::stream> out(new multiplexing::publisher);
  out->write(r);

  // Set all the BAs to should not be rebuild.
  {
    std::string query = "UPDATE cfg_bam"
                        "  SET must_be_rebuild='0'";
    database_query q(_db);
    q.run_query(
        query,
        "BAM: could not update the list of BAs to rebuild");
  }
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void monitoring_stream::_update_status(std::string const& status) {
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}
