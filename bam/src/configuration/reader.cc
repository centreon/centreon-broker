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

#include <cassert>
#include <cstdlib>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include <memory>
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"
#include "com/centreon/broker/bam/sql_mapping.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/time/timeperiod.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] db  Database connection.
 */
reader::reader(QSqlDatabase* db) : _db(db) {}

/**
 *  Destructor.
 */
reader::~reader() {}

/**
 *  Read configuration from database.
 *
 *  @param[out] st  All the configuration state for the BA subsystem
 *                  recuperated from the specified database.
 */
void reader::read(state& st) {
  try {
    _db->transaction(); // A single explicit transaction is more efficient.
    _load(st.get_bas());
    _load(st.get_kpis());
    _load(st.get_bool_exps());
    _load(st.get_meta_services());
    _load(st.get_mapping());
    _load_dimensions();
    _db->rollback();
  }
  catch (std::exception const& e) {
    st.clear();
    throw ;
  }
  return ;
}

/**
 *  @brief Copy constructor
 *
 *  Hidden implementation, never called.
 */
reader::reader(reader const& other) : _db(NULL) {
  (void)other;
  assert(!"BAM configuration reader is not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Hidden implementation, never called.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
reader& reader::operator=(reader const& other) {
  (void)other;
  assert(!"BAM configuration reader is not copyable");
  abort();
  return (*this);
}

/**
 *  Load KPIs from the DB.
 *
 *  @param[out] kpis The list of kpis in database.
 */
void reader::_load(state::kpis& kpis) {
  QSqlQuery query(_db->exec(
    "SELECT  k.kpi_id, k.state_type, k.host_id, k.service_id, k.id_ba,"
    "        k.id_indicator_ba, k.current_status, k.last_level,"
    "        k.downtime, k.acknowledged, k.ignore_downtime,"
    "        k.ignore_acknowledged,"
    "        COALESCE(k.drop_warning, ww.impact),"
    "        COALESCE(k.drop_critical, cc.impact),"
    "        COALESCE(k.drop_unknown, uu.impact),"
    "        k.last_state_change, k.in_downtime"
    "  FROM  mod_bam_kpi AS k"
    "  LEFT JOIN mod_bam_impacts AS ww"
    "    ON k.drop_warning_impact_id = ww.id_impact"
    "  LEFT JOIN mod_bam_impacts AS cc"
    "    ON k.drop_critical_impact_id = cc.id_impact"
    "  LEFT JOIN mod_bam_impacts AS uu"
    "    ON k.drop_unknown_impact_id = uu.id_impact"
    "  WHERE k.activate='1' AND k.boolean_id IS NULL"));
  if (query.lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve KPI configuration from DB: "
           << query.lastError().text());

  while (query.next()) {
    // KPI object.
    unsigned int kpi_id(query.value(0).toUInt());
    kpis[kpi_id] =
      kpi(
        kpi_id, // ID.
        query.value(1).toInt(), // State type.
        query.value(2).toUInt(), // Host ID.
        query.value(3).toUInt(), // Service ID.
        query.value(4).toUInt(), // BA ID.
        query.value(5).toUInt(), // BA indicator ID.
        query.value(6).toInt(), // Status.
        query.value(7).toInt(), // Hard state.
        query.value(8).toFloat(), // Downtimed.
        query.value(9).toFloat(), // Acknowledged.
        query.value(10).toBool(), // Ignore downtime.
        query.value(11).toBool(), // Ignore acknowledgement.
        query.value(12).toDouble(), // Warning.
        query.value(13).toDouble(), // Critical.
        query.value(14).toDouble()); // Unknown.

    // KPI state.
    if (!query.value(15).isNull()) {
      kpi_event e;
      e.kpi_id = kpi_id;
      e.status = query.value(6).toInt();
      e.start_time = query.value(15).toLongLong();
      e.in_downtime = query.value(16).toBool();
      kpis[kpi_id].set_opened_event(e);
    }
  }

  return ;
}

/**
 *  Load BAs from the DB.
 *
 *  @param[out] bas The list of BAs in database.
 */
void reader::_load(state::bas& bas) {
  QSqlQuery query(_db->exec(
    "SELECT ba_id, name, level_w, level_c, last_state_change, "
    "       current_status, in_downtime"
    "  FROM mod_bam"
    "  WHERE activate='1'"));
  if (query.lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve BA configuration from DB: "
           << query.lastError().text());

  while (query.next()) {
    // BA object.
    unsigned int ba_id(query.value(0).toUInt());
    bas[ba_id] =
      ba(
        ba_id, // ID.
        query.value(1).toString().toStdString(), // Name.
        query.value(2).toFloat(), // Warning level.
        query.value(3).toFloat()); // Critical level.

    // BA state.
    if (!query.value(4).isNull()) {
      ba_event e;
      e.ba_id = ba_id;
      e.start_time = query.value(4).toLongLong();
      e.status = query.value(5).toInt();
      e.in_downtime = query.value(6).toBool();
      bas[ba_id].set_opened_event(e);
    }
  }

  // Load the associated ba_id from the table services.
  // All the associated services have for description 'ba_[id]'.
  query = _db->exec(
                 "SELECT s.service_description, hsr.host_host_id, hsr.service_service_id"
                 "  FROM service AS s"
                 "  INNER JOIN host_service_relation AS hsr"
                 "  ON s.service_id=hsr.service_service_id"
                 "  WHERE service_description LIKE 'ba_%'");
  if (query.lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve BA service IDs from DB: "
           << query.lastError().text());

  while (query.next()) {
    std::string service_description = query.value(0).toString().toStdString();
    QString host_id = query.value(1).toString();
    QString service_id = query.value(2).toString();
    service_description.erase(0, strlen("ba_"));

    if (!service_description.empty()) {
      bool ok = false;
      unsigned int ba_id = QString(service_description.c_str()).toUInt(&ok);
      if (!ok) {
        logging::error(logging::medium)
          << "BAM: BA host:" << host_id
          << "service: " << service_id
          << "unknown when attempting to retrieve services";
        continue;
      }
      state::bas::iterator found = bas.find(ba_id);
      if (found == bas.end()) {
        logging::error(logging::medium)
          << "BAM: BA service '"
          << query.value(0).toString()
          << "' not found when attempting to retrieve services";
        continue;
      }
      found->second.set_host_id(host_id.toUInt());
      found->second.set_service_id(service_id.toUInt());
    }
  }

  // Test for ba without service id.
  for (state::bas::const_iterator it(bas.begin()),
                                  end(bas.end());
       it != end;
       ++it)
    if (it->second.get_service_id() == 0)
      throw (reader_exception() << "BAM: BA " << it->second.get_id()
             << " has no associated service");

  // Load the timeperiods associated with the BAs.
  query = _db->exec("SELECT ba_id, timeperiod_id, is_default FROM mod_bam_ba_tp_rel");
  if (query.lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve the timeperiods associated with the BAs: "
           << query.lastError().text());
  while (query.next()) {
    unsigned int ba_id = query.value(0).toInt();
    state::bas::iterator found = bas.find(ba_id);
    if (found == bas.end()) {
      logging::error(logging::medium)
        << "BAM: ba id: "
        << ba_id
        << "not found when reading timeperiod relations from db.";
    }
    // This is a default timeperiod.
    if (query.value(2).toBool())
      found->second.set_default_timeperiod(query.value(1).toInt());
    // This is not a default timeperiod.
    else
      found->second.add_timeperiod(query.value(1).toInt());
  }
  return ;
}

/**
 *  Load boolean expressions from the DB.
 *
 *  @param[out] bool_exps The list of bool expression in database.
 */
void reader::_load(state::bool_exps& bool_exps) {
  // Load boolean expressions themselves.
  {
    QSqlQuery query(_db->exec(
      "SELECT  be.boolean_id, COALESCE(be.impact, imp.impact),"
      "        be.expression, be.bool_state"
      "  FROM  mod_bam_boolean as be"
      "  LEFT JOIN mod_bam_impacts as imp"
      "    ON be.impact_id = imp.id_impact"));
    if (query.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve boolean expression "
             << "configuration from DB: " << query.lastError().text());

    while (query.next()) {
      bool_exps[query.value(0).toInt()] =
                  bool_expression(
                    query.value(0).toInt(), // ID.
                    query.value(1).toFloat(),// Impact.
                    query.value(2).toString().toStdString(), // Expression.
                    query.value(3).toBool()); // Impact if.
    }
  }

  // Load relations of boolean expressions with BAs.
  {
    std::map<unsigned int, bool_expression::ids_of_bas> impacted_bas;
    {
      QSqlQuery q(_db->exec(
        "SELECT boolean_id, ba_id FROM mod_bam_bool_rel"));
      if (q.lastError().isValid())
        throw (reader_exception()
               << "BAM: could not retrieve BAs impacted by boolean "
               << "expressions: " << q.lastError().text());

      while (q.next())
        impacted_bas[q.value(0).toUInt()].push_back(q.value(1).toUInt());
    }
    for (state::bool_exps::iterator
           it(bool_exps.begin()), end(bool_exps.end());
         it != end;
         ++it) {
      std::map<unsigned int, bool_expression::ids_of_bas>::iterator
        it_impacted_bas(impacted_bas.find(it->first));
      if (it_impacted_bas != impacted_bas.end())
        it->second.impacted_bas().swap(it_impacted_bas->second);
      else
        it->second.impacted_bas().clear();
    }
  }

  // Load kpi_id associated with boolean BAs.
  {
    QSqlQuery q(_db->exec(
      "SELECT boolean_id, kpi_id, last_state_change,"
      "       current_status, in_downtime"
      "  FROM mod_bam_kpi"
      "  WHERE activate='1' AND boolean_id IS NOT NULL"));
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve the KPI IDs of the boolean "
                "expressions: " << q.lastError().text());
    while (q.next()) {
      // Boolean KPI ID.
      unsigned int boolean_id = q.value(0).toUInt();
      unsigned int kpi_id = q.value(1).toUInt();
      state::bool_exps::iterator found = bool_exps.find(boolean_id);
      if (found == bool_exps.end())
        throw (reader_exception()
               << "BAM: found a KPI pointing to an inexisting boolean of ID "
               << boolean_id);
      found->second.set_kpi_id(kpi_id);
      if (!q.value(2).isNull()
          && (q.value(2).toLongLong()
              > found->second.get_opened_event().start_time)) {
        kpi_event e;
        e.start_time = q.value(2).toLongLong();
        e.status = q.value(3).toInt();
        e.in_downtime = q.value(4).toBool();
        found->second.set_opened_event(e);
      }
    }
  }

  return ;
}

/**
 *  Load meta-services from the DB.
 *
 *  @param[out] meta_services  Meta-services.
 */
void reader::_load(state::meta_services& meta_services) {
  // Load meta-services.
  {
    QSqlQuery q(_db->exec(
      "SELECT meta_id, meta_name, calcul_type, warning, critical,"
      "       meta_select_mode, regexp_str, metric"
      "  FROM meta_service"
      "  WHERE meta_activate='1'"));
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve meta-services: "
             << q.lastError().text());
    while (q.next())
      meta_services.push_back(meta_service(
                                q.value(0).toUInt(),
                                q.value(1).toString().toStdString(),
                                q.value(2).toString().toStdString(),
                                q.value(3).toDouble(),
                                q.value(4).toDouble(),
                                (q.value(5).toInt() == 2
                                ? q.value(6).toString().toStdString()
                                : ""),
                                (q.value(5).toInt() == 2
                                 ? q.value(7).toString().toStdString()
                                 : "")));
  }

  // Load metrics of meta-services.
  for (state::meta_services::iterator
         it(meta_services.begin()),
         end(meta_services.end());
       it != end;
       ++it) {
    // SQL LIKE mode.
    if (!it->get_service_filter().empty()
        && !it->get_metric_name().empty()) {
      std::ostringstream query;
      query << "SELECT m.metric_id"
            << "  FROM metrics AS m"
            << "    INNER JOIN index_data AS i"
            << "    ON m.index_id=i.id"
            << "    INNER JOIN services AS s"
            << "    ON i.host_id=s.host_id AND i.service_id=s.service_id"
            << "  WHERE s.description LIKE '" << it->get_service_filter() << "'"
            << "    AND m.metric_name='" << it->get_metric_name() << "'";
      // XXX : we do not have access to the centreon_storage DB
      // QSqlQuery q(_db->exec(query.str().c_str()));
      // if (q.lastError().isValid())
      //   throw (reader_exception()
      //          << "BAM: could not retrieve members of meta-service '"
      //          << it->get_name() << "': " << q.lastError().text());
      // while (q.next())
      //   it->add_metric(q.value(0).toUInt());
    }
    // Service list mode.
    else {
      std::ostringstream query;
      query << "SELECT metric_id"
            << "  FROM meta_service_relation"
            << "  WHERE meta_id=" << it->get_id()
            << "    AND activate='1'";
      QSqlQuery q(_db->exec(query.str().c_str()));
      if (q.lastError().isValid())
        throw (reader_exception()
               << "BAM: could not retrieve members of meta-service '"
               << it->get_name() << "': " << q.lastError().text());
      while (q.next())
        it->add_metric(q.value(0).toUInt());
    }
  }
  return ;
}

/**
 *  Load host/service IDs from the DB.
 *
 *  @param[out] mapping  Host/service mapping.
 */
void reader::_load(bam::hst_svc_mapping& mapping) {
  // XXX : expand hostgroups and servicegroups
  QSqlQuery q(_db->exec(
    "SELECT h.host_id, s.service_id, h.host_name, s.service_description"
    "  FROM service AS s"
    "  LEFT JOIN host_service_relation AS hsr"
    "    ON s.service_id=hsr.service_service_id"
    "  LEFT JOIN host AS h"
    "    ON hsr.host_host_id=h.host_id"));
  if (q.lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve host/service IDs: "
           << q.lastError().text());
  while (q.next())
    mapping.set_service(
              q.value(2).toString().toStdString(),
              q.value(3).toString().toStdString(),
              q.value(0).toUInt(),
              q.value(1).toUInt());
  return ;
}

/**
 *  Load the dimensions from the database.
 */
void reader::_load_dimensions() {
  std::auto_ptr<io::stream> out(new multiplexing::publisher);
  // As this operation is destructive (it truncates the database),
  // we cache the data until we are sure we have all the data
  // needed from the database.
  std::vector<misc::shared_ptr<io::data> > datas;
  datas.push_back(misc::shared_ptr<io::data>(
                    new dimension_truncate_table_signal));

  // Get the data from the db.
  QSqlQuery q(*_db);
  q.setForwardOnly(true);

  // Load the BAs.
  std::map<unsigned int, misc::shared_ptr<dimension_ba_event> > bas;
  {
    q.exec(
        "SELECT ba_id, name, description,"
        "       sla_month_percent_warn, sla_month_percent_crit,"
        "       sla_month_duration_warn, sla_month_duration_crit"
        "  FROM mod_bam");
    if (q.lastError().isValid())
      throw (reader_exception() << "BAM: could not retrieve BA list: "
             << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<dimension_ba_event> ba(new dimension_ba_event);
      ba->ba_id = q.value(0).toUInt();
      ba->ba_name = q.value(1).toString().toStdString();
      ba->ba_description = q.value(2).toString().toStdString();
      ba->sla_month_percent_1 = q.value(3).toDouble();
      ba->sla_month_percent_2 = q.value(4).toDouble();
      ba->sla_duration_1 = q.value(5).toInt();
      ba->sla_duration_2 = q.value(6).toInt();
      datas.push_back(ba.staticCast<io::data>());
      bas[ba->ba_id] = ba;
    }
  }

  // Load the BVs.
  {
    q.exec("SELECT id_ba_group, ba_group_name, ba_group_description"
           "  FROM mod_bam_ba_groups");
    if (q.lastError().isValid())
      throw (reader_exception() << "BAM: could not retrieve BV list: "
             << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<dimension_bv_event>
          bv(new dimension_bv_event);
      bv->bv_id = q.value(0).toInt();
      bv->bv_name = q.value(1).toString().toStdString();
      bv->bv_description = q.value(2).toString().toStdString();
      datas.push_back(bv.staticCast<io::data>());
    }
  }

  // Load the BA BV relations.
  {
    q.exec("SELECT id_ba, id_ba_group"
           "  FROM mod_bam_bagroup_ba_relation");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve BV memberships of BAs: "
             << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<dimension_ba_bv_relation_event>
          babv(new dimension_ba_bv_relation_event);
      babv->ba_id = q.value(0).toInt();
      babv->bv_id = q.value(1).toInt();
      datas.push_back(babv.staticCast<io::data>());
    }
  }

  // Load the KPIs
  // Unfortunately, we need to get the names of the service/host/meta_service
  // /ba/boolean expression associated with this KPI.
  // This explains the numerous joins.
  {
    q.exec("SELECT k.kpi_id, k.kpi_type, k.host_id, k.service_id, k.id_ba,"
           "       k.id_indicator_ba, k.meta_id, k.boolean_id,"
           "        COALESCE(k.drop_warning, ww.impact),"
           "        COALESCE(k.drop_critical, cc.impact),"
           "        COALESCE(k.drop_unknown, uu.impact),"
           "       h.host_name, s.service_description, b.name,"
           "       meta.meta_name, boo.name"
           "  FROM  mod_bam_kpi AS k"
           "  LEFT JOIN mod_bam_impacts AS ww"
           "    ON k.drop_warning_impact_id = ww.id_impact"
           "  LEFT JOIN mod_bam_impacts AS cc"
           "    ON k.drop_critical_impact_id = cc.id_impact"
           "  LEFT JOIN mod_bam_impacts AS uu"
           "    ON k.drop_unknown_impact_id = uu.id_impact"
           "  LEFT JOIN host AS h"
           "    ON h.host_id = k.host_id"
           "  LEFT JOIN service AS s"
           "    ON s.service_id = k.service_id"
           "  INNER JOIN mod_bam AS b"
           "    ON b.ba_id = k.id_ba"
           "  LEFT JOIN meta_service AS meta"
           "    ON meta.meta_id = k.meta_id"
           "  LEFT JOIN mod_bam_boolean as boo"
           "    ON boo.boolean_id = k.boolean_id");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve kpi dimensions: "
             << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<dimension_kpi_event> k(new dimension_kpi_event);
      k->kpi_id = q.value(0).toInt();
      k->host_id = q.value(2).toInt();
      k->service_id = q.value(3).toInt();
      k->ba_id = q.value(4).toInt();
      k->kpi_ba_id = q.value(5).toInt();
      k->meta_service_id = q.value(6).toInt();
      k->boolean_id = q.value(7).toInt();
      k->impact_warning = q.value(8).toDouble();
      k->impact_critical = q.value(9).toDouble();
      k->impact_unknown = q.value(10).toDouble();
      k->host_name = q.value(11).toString().toStdString();
      k->service_description = q.value(12).toString().toStdString();
      k->ba_name = q.value(13).toString().toStdString();
      k->meta_service_name = q.value(14).toString().toStdString();
      k->boolean_name = q.value(15).toString().toStdString();

      // Resolve the id_indicator_ba
      if (k->kpi_ba_id) {
        std::map<unsigned int,
                 misc::shared_ptr<dimension_ba_event> >::const_iterator
            found = bas.find(k->kpi_ba_id);
        if (found == bas.end())
          throw (reader_exception()
                 << "BAM: could not retrieve BA " << k->kpi_ba_id
                 << " used as KPI " << k->kpi_id);
        k->kpi_ba_name = found->second->ba_name;
      }
      datas.push_back(k.staticCast<io::data>());
    }
  }

  // Load the timeperiods.
  std::map<unsigned int, time::timeperiod::ptr> timeperiods;
  {
    // Load the timeperiod data.
    q.exec("SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday, tp_tuesday, "
           "tp_wednesday, tp_thursday, tp_friday, tp_saturday"
           "  FROM timeperiod");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve timeperiods: "
             << q.lastError().text());
    while (q.next())
      timeperiods[q.value(0).toInt()] = time::timeperiod::ptr(
        new time::timeperiod(
              q.value(0).toInt(), // Id
              q.value(1).toString().toStdString(), // name
              q.value(2).toString().toStdString(), // alias
              q.value(3).toString().toStdString(), // sunday
              q.value(4).toString().toStdString(), // monday
              q.value(5).toString().toStdString(), // tuesday
              q.value(6).toString().toStdString(), // wednesday
              q.value(7).toString().toStdString(), // thursday
              q.value(8).toString().toStdString(), // friday
              q.value(9).toString().toStdString()  // saturday
              ));

    // Load the exclusions
    q.exec("SELECT timeperiod_id, days, timerange"
           "  FROM timeperiod_exceptions");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve timeperiod exceptions: "
             << q.lastError().text());
    while (q.next()) {
      unsigned int timeperiod_id = q.value(0).toInt();
      std::map<unsigned int, time::timeperiod::ptr>::iterator found
          = timeperiods.find(timeperiod_id);
      if (found == timeperiods.end())
        throw (reader_exception())
                << "BAM: Found a timeperiod exception pointing to an "
                   "inexisting timeperiod (timeperiod id = " << timeperiod_id;
      found->second->add_exception(q.value(1).toString().toStdString(),
                                  q.value(2).toString().toStdString());

    }

    // Load the excluded timeperiods.
    q.exec("SELECT timeperiod_id, timeperiod_exclude_id"
           "  FROM timeperiod_exclude_relations");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve timeperiod exclude relations: "
             << q.lastError().text());
    while (q.next()) {
      unsigned int timeperiod_id = q.value(0).toInt();
      unsigned int timeperiod_exclude_id = q.value(1).toInt();
      std::map<unsigned int, time::timeperiod::ptr>::iterator found
          = timeperiods.find(timeperiod_id);
      if (found == timeperiods.end())
        throw (reader_exception())
                << "BAM: Found a timeperiod exclude pointing to an inexisting "
                   "timeperiod (timeperiod id = " << timeperiod_id;
      std::map<unsigned int, time::timeperiod::ptr>::iterator found_excluded =
                          timeperiods.find(timeperiod_exclude_id);
      if (found_excluded == timeperiods.end())
        throw (reader_exception())
                << "BAM: Found a timeperiod exclude pointing to an inexisting "
                   "excluded timeperiod (excluded timeperiod id = "
                << timeperiod_exclude_id;
      found->second->add_excluded(found_excluded->second);
    }
  }

  // Create the timeperiod events.
  for (std::map<unsigned int, time::timeperiod::ptr>::iterator
         it(timeperiods.begin()),
         end(timeperiods.end());
       it != end;
       ++it) {
    misc::shared_ptr<dimension_timeperiod> dtp(new dimension_timeperiod);
    dtp->timeperiod = it->second;
    datas.push_back(dtp);
  }

  // Load the ba-timeperiods relations.
  {
    q.exec("SELECT ba_id, timeperiod_id, is_default FROM mod_bam_ba_tp_rel");
    if (q.lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve the timeperiods associated "
                "with the BAs: "
             << q.lastError().text());
    while (q.next()) {
      misc::shared_ptr<dimension_ba_timeperiod_relation>
        dbtr(new dimension_ba_timeperiod_relation);
      dbtr->ba_id = q.value(0).toInt();
      dbtr->timeperiod_id = q.value(1).toInt();
      dbtr->is_default = q.value(2).toBool();
      datas.push_back(dbtr);
    }
  }

  // Write all the cached data to the publisher.
  for (std::vector<misc::shared_ptr<io::data> >::iterator it(datas.begin()),
                                                          end(datas.end());
       it != end;
       ++it)
    out->write(*it);
}
