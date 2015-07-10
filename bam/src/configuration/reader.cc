/*
** Copyright 2014-2015 Merethis
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

#include <cstdlib>
#include <sstream>
#include <memory>
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] centreon_db  Centreon database connection.
 */
reader::reader(database& centreon_db) : _db(centreon_db) {}

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
    _load_dimensions();
    _load(st.get_bas());
    _load(st.get_kpis());
    _load(st.get_bool_exps());
    _load(st.get_meta_services());
    _load(st.get_hst_svc_mapping());
  }
  catch (std::exception const& e) {
    st.clear();
    throw ;
  }
  return ;
}

/**
 *  Load KPIs from the DB.
 *
 *  @param[out] kpis The list of kpis in database.
 */
void reader::_load(state::kpis& kpis) {
  try {
    database_query query(_db);
    query.run_query(
      "SELECT  k.kpi_id, k.state_type, k.host_id, k.service_id, k.id_ba,"
      "        k.id_indicator_ba, k.meta_id, k.boolean_id,"
      "        k.current_status, k.last_level, k.downtime,"
      "        k.acknowledged, k.ignore_downtime,"
      "        k.ignore_acknowledged,"
      "        COALESCE(COALESCE(k.drop_warning, ww.impact), g.average_impact),"
      "        COALESCE(COALESCE(k.drop_critical, cc.impact), g.average_impact),"
      "        COALESCE(COALESCE(k.drop_unknown, uu.impact), g.average_impact),"
      "        k.last_state_change, k.in_downtime, k.last_impact"
      "  FROM  cfg_bam_kpi AS k"
      "  LEFT JOIN cfg_bam AS mb"
      "     ON k.id_ba = mb.ba_id"
      "  LEFT JOIN cfg_bam_impacts AS ww"
      "    ON k.drop_warning_impact_id = ww.id_impact"
      "  LEFT JOIN cfg_bam_impacts AS cc"
      "    ON k.drop_critical_impact_id = cc.id_impact"
      "  LEFT JOIN cfg_bam_impacts AS uu"
      "    ON k.drop_unknown_impact_id = uu.id_impact"
      "  LEFT JOIN (SELECT id_ba, 100.0 / COUNT(kpi_id) AS average_impact"
      "               FROM cfg_bam_kpi"
      "               WHERE activate='1'"
      "               GROUP BY id_ba) AS g"
      "    ON k.id_ba=g.id_ba"
      "  WHERE k.activate='1'"
      "    AND mb.activate='1'");
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
          query.value(6).toUInt(), // Meta-service ID.
          query.value(7).toUInt(), // Boolean expression ID.
          query.value(8).toInt(), // Status.
          query.value(9).toInt(), // Last level.
          query.value(10).toFloat(), // Downtimed.
          query.value(11).toFloat(), // Acknowledged.
          query.value(12).toBool(), // Ignore downtime.
          query.value(13).toBool(), // Ignore acknowledgement.
          query.value(14).toDouble(), // Warning.
          query.value(15).toDouble(), // Critical.
          query.value(16).toDouble()); // Unknown.

      // KPI state.
      if (!query.value(17).isNull()) {
        kpi_event e;
        e.kpi_id = kpi_id;
        e.status = query.value(8).toInt();
        e.start_time = query.value(17).toLongLong();
        e.in_downtime = query.value(18).toBool();
        e.impact_level = query.value(19).isNull() ?
                                 -1 : query.value(19).toDouble();
        kpis[kpi_id].set_opened_event(e);
      }
    }

    // Load host ID/service ID of meta-services (temporary fix until
    // Centreon Broker 3 where meta-services will be computed by Broker
    // itself.
    for (state::kpis::iterator it(kpis.begin()), end(kpis.end());
         it != end;
         ++it) {
      if (it->second.is_meta()) {
        std::ostringstream oss;
        oss << "SELECT hsr.host_host_id, hsr.service_service_id"
               "  FROM cfg_services AS s"
               "  LEFT JOIN cfg_hosts_services_relations AS hsr"
               "    ON s.service_id=hsr.service_service_id"
               "  WHERE s.service_description='meta_" << it->second.get_meta_id()
            << "'";
        query.run_query(
                oss.str(),
                "could not virtual meta-service's service");
        if (!query.next())
          throw (exceptions::msg() << "virtual service of meta-service "
                 << it->first << " does not exist");
        it->second.set_host_id(query.value(0).toUInt());
        it->second.set_service_id(query.value(1).toUInt());
      }
    }
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve KPI configuration from DB: "
           << e.what());
  }
  return ;
}

/**
 *  Load BAs from the DB.
 *
 *  @param[out] bas  The list of BAs in database.
 */
void reader::_load(state::bas& bas) {
  try {
    database_query query(_db);
    query.run_query(
      "SELECT ba_id, name, level_w, level_c, last_state_change, "
      "       current_status, in_downtime"
      "  FROM cfg_bam"
      "  WHERE activate='1'");
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

    // Get organization ID.
    unsigned int organization_id;
    {
      std::ostringstream query;
      query << "SELECT o.organization_id"
               "  FROM cfg_pollers AS p"
               "  INNER JOIN cfg_organizations AS o"
               "    ON p.organization_id=o.organization_id"
               "  WHERE p.poller_id="
            << config::applier::state::instance().poller_id();
      database_query q(_db);
      q.run_query(query.str());
      if (q.next())
        organization_id = q.value(0).toUInt();
      else {
        while (1) {
          q.run_query("SELECT organization_id FROM cfg_organizations");
          if (q.next()) {
            organization_id = q.value(0).toUInt();
            break ;
          }
          q.run_query(
            "INSERT INTO cfg_organizations (name, shortname, active)"
            "  VALUES ('Default organization', 'default_organization',"
            "          1)");
        }
      }
    }

    // Get host ID.
    unsigned int host_id;
    while (1) {
      {
        std::ostringstream query;
        query << "SELECT host_id"
                 "  FROM cfg_hosts"
                 "  WHERE host_name='_Module_BAM'"
                 "    AND organization_id=" << organization_id;
        database_query q(_db);
        q.run_query(query.str());
        if (q.next()) {
          host_id = q.value(0).toUInt();
          break ;
        }
      }
      {
        std::ostringstream query;
        query << "INSERT INTO cfg_hosts (host_name, organization_id)"
                 "  VALUES ('_Module_BAM', " << organization_id << ")";
        database_query q(_db);
        q.run_query(query.str());
      }
    }

    // Load service ID for each BA.
    for (state::bas::iterator it(bas.begin()), end(bas.end());
         it != end;
         ++it) {
      unsigned int service_id;
      while (1) {
        {
          std::ostringstream query;
          query << "SELECT service_id"
                   "  FROM cfg_services"
                   "  WHERE service_description='ba_"
                << it->second.get_id() << "'"
                   "    AND organization_id=" << organization_id;
          database_query q(_db);
          q.run_query(query.str());
          if (q.next()) {
            service_id = q.value(0).toUInt();
            break ;
          }
        }
        {
          std::ostringstream query;
          query << "INSERT INTO cfg_services (service_description,"
                   "            organization_id)"
                   "  VALUES ('ba_" << it->second.get_id() << "', "
                << organization_id << ")";
          database_query q(_db);
          q.run_query(query.str());
        }
      }

      it->second.set_host_id(host_id);
      it->second.set_service_id(service_id);
    }
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve BA configuration from DB: "
           << e.what());
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
  try {
    database_query query(_db);
    query.run_query(
      "SELECT  boolean_id, expression, bool_state"
      "  FROM  cfg_bam_boolean"
      "  WHERE activate=1");
    while (query.next()) {
      bool_exps[query.value(0).toUInt()] =
                  bool_expression(
                    query.value(0).toUInt(), // ID.
                    query.value(1).toString().toStdString(), // Expression.
                    query.value(2).toBool()); // Impact if.
    }
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve boolean expression "
           << "configuration from DB: " << e.what());
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
  try {
    database_query q(_db);
    q.run_query(
      "SELECT meta_id, meta_name, calcul_type, warning, critical,"
      "       meta_select_mode, regexp_str, metric"
      "  FROM cfg_meta_services"
      "  WHERE meta_activate='1'");
    while (q.next()) {
      unsigned int meta_id(q.value(0).toUInt());
      meta_services[meta_id] =
        meta_service(
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
           : ""));
    }
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve meta-services: "
           << e.what());
  }

  // Load host_id/service_id of virtual meta-service services. All
  // associated services have for description 'meta_[id]'.
  try {
    database_query q(_db);
    q.run_query(
      "SELECT h.host_name, s.service_description,"
      "       hsr.host_host_id, hsr.service_service_id"
      "  FROM cfg_services AS s"
      "  INNER JOIN cfg_hosts_services_relations AS hsr"
      "    ON s.service_id=hsr.service_service_id"
      "  INNER JOIN cfg_hosts AS h"
      "    ON hsr.host_host_id=h.host_id"
      "  WHERE s.service_description LIKE 'meta_%'");
    while (q.next()) {
      unsigned int host_id(q.value(2).toUInt());
      unsigned int service_id(q.value(3).toUInt());
      std::string service_description(q.value(1).toString().toStdString());
      service_description.erase(0, strlen("meta_"));
      bool ok(false);
      unsigned int meta_id(QString(service_description.c_str()).toUInt(&ok));
      if (!ok) {
        logging::info(logging::medium)
          << "BAM: service '" << q.value(1).toString() << "' of host '"
          << q.value(0).toString()
          << "' is not a valid virtual meta-service service";
        continue ;
      }
      state::meta_services::iterator found(meta_services.find(meta_id));
      if (found == meta_services.end()) {
        logging::info(logging::medium)
          << "BAM: virtual meta-service service '"
          << q.value(1).toString() << "' of host '"
          << q.value(0).toString()
          << "' references an unknown meta-service (" << meta_id << ")";
        continue ;
      }
      found->second.set_host_id(host_id);
      found->second.set_service_id(service_id);
    }
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve meta-services' service IDs from DB: "
           << e.what());
  }

  // Check for meta-services without service ID.
  for (state::meta_services::const_iterator
         it(meta_services.begin()),
         end(meta_services.end());
       it != end;
       ++it)
    if (!it->second.get_host_id() || !it->second.get_service_id())
      throw (reader_exception() << "BAM: meta-service "
             << it->first << " has no associated virtual service");

  // Load metrics of meta-services.
  for (state::meta_services::iterator
         it(meta_services.begin()),
         end(meta_services.end());
       it != end;
       ++it) {
    // SQL LIKE mode.
    if (!it->second.get_service_filter().empty()
        && !it->second.get_metric_name().empty()) {
      std::ostringstream query;
      query << "SELECT m.metric_id"
            << "  FROM rt_metrics AS m"
            << "    INNER JOIN rt_index_data AS i"
            << "    ON m.index_id=i.id"
            << "    INNER JOIN rt_services AS s"
            << "    ON i.host_id=s.host_id AND i.service_id=s.service_id"
            << "  WHERE s.description LIKE '"
            << it->second.get_service_filter() << "'"
            << "    AND m.metric_name='"
            << it->second.get_metric_name() << "'";
      database_query q(_db);
      try { q.run_query(query.str()); }
      catch (std::exception const& e) {
        throw (reader_exception()
               << "BAM: could not retrieve members of meta-service '"
               << it->second.get_name() << "': " << e.what());
      }
      while (q.next())
        it->second.add_metric(q.value(0).toUInt());
    }
    // Service list mode.
    else {
      try {
        std::ostringstream query;
        query << "SELECT metric_id"
              << "  FROM cfg_meta_services_relations"
              << "  WHERE meta_id=" << it->second.get_id()
              << "    AND activate='1'";
        database_query q(_db);
        q.run_query(query.str());
        while (q.next())
          it->second.add_metric(q.value(0).toUInt());
      }
      catch (reader_exception const& e) {
        (void)e;
        throw ;
      }
      catch (std::exception const& e) {
        throw (reader_exception()
               << "BAM: could not retrieve members of meta-service '"
               << it->second.get_name() << "': " << e.what());
      }
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
  try {
    database_query q(_db);
    q.run_query(
      "SELECT h.host_id, s.service_id, h.host_name, s.service_description,"
          "   service_activate"
      "  FROM cfg_services AS s"
      "  LEFT JOIN cfg_hosts_services_relations AS hsr"
      "    ON s.service_id=hsr.service_service_id"
      "  LEFT JOIN cfg_hosts AS h"
      "    ON hsr.host_host_id=h.host_id");
    while (q.next())
      mapping.set_service(
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                q.value(0).toUInt(),
                q.value(1).toUInt(),
                q.value(4).toString() == "1");
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not retrieve host/service IDs: "
           << e.what());
  }
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
  misc::shared_ptr<dimension_truncate_table_signal> dtts(
                      new dimension_truncate_table_signal);
  dtts->update_started = true;
  datas.push_back(dtts);

  // Get the data from the DB.
  database_query q(_db);

  // Load the dimensions.
  std::map<unsigned int, time::timeperiod::ptr> timeperiods;
  std::map<unsigned int, misc::shared_ptr<dimension_ba_event> > bas;
  try {
    // Load the timeperiods themselves.
    q.run_query(
      "SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday, tp_tuesday, "
      "tp_wednesday, tp_thursday, tp_friday, tp_saturday"
      "  FROM cfg_timeperiods",
      "could not load timeperiods from the database");
    while (q.next()) {
      timeperiods[q.value(0).toUInt()] = time::timeperiod::ptr(
        new time::timeperiod(
              q.value(0).toUInt(),                   // id
              q.value(1).toString().toStdString(),   // name
              q.value(2).toString().toStdString(),   // alias
              q.value(3).toString().toStdString(),   // sunday
              q.value(4).toString().toStdString(),   // monday
              q.value(5).toString().toStdString(),   // tuesday
              q.value(6).toString().toStdString(),   // wednesday
              q.value(7).toString().toStdString(),   // thursday
              q.value(8).toString().toStdString(),   // friday
              q.value(9).toString().toStdString())); // saturday
      misc::shared_ptr<dimension_timeperiod> tp(new dimension_timeperiod);
      tp->id = q.value(0).toUInt();
      tp->name = q.value(1).toString();
      tp->sunday = q.value(3).toString();
      tp->monday = q.value(4).toString();
      tp->tuesday = q.value(5).toString();
      tp->wednesday = q.value(6).toString();
      tp->thursday = q.value(7).toString();
      tp->friday = q.value(8).toString();
      tp->saturday = q.value(9).toString();
      datas.push_back(tp.staticCast<io::data>());
    }

    // Load the timeperiod exceptions.
    q.run_query(
      "SELECT timeperiod_id, days, timerange"
      "  FROM cfg_timeperiods_exceptions",
      "could not retrieve timeperiod exceptions from the database");
    while (q.next()) {
      unsigned int timeperiod_id = q.value(0).toUInt();
      std::map<unsigned int, time::timeperiod::ptr>::iterator found
          = timeperiods.find(timeperiod_id);
      if (found == timeperiods.end())
        throw (reader_exception()
               << "BAM: found a timeperiod exception pointing to an "
               "inexisting timeperiod (timeperiod ID is " << timeperiod_id
               << ")");
      found->second->add_exception(
                       q.value(1).toString().toStdString(),
                       q.value(2).toString().toStdString());
      misc::shared_ptr<dimension_timeperiod_exception>
        exception(new dimension_timeperiod_exception);
      exception->timeperiod_id = timeperiod_id;
      exception->daterange = q.value(1).toString();
      exception->timerange = q.value(2).toString();
      datas.push_back(exception.staticCast<io::data>());
    }

    // Load the excluded timeperiods.
    q.run_query(
      "SELECT timeperiod_id, timeperiod_exclude_id"
      "  FROM cfg_timeperiods_exclude_relations",
      "could not retrieve timeperiod exclusions from the database");
    while (q.next()) {
      unsigned int timeperiod_id = q.value(0).toUInt();
      unsigned int timeperiod_exclude_id = q.value(1).toUInt();
      std::map<unsigned int, time::timeperiod::ptr>::iterator found
          = timeperiods.find(timeperiod_id);
      if (found == timeperiods.end())
        throw (reader_exception()
               << "BAM: found a timeperiod exclude pointing to an inexisting "
               "timeperiod (timeperiod has ID " << timeperiod_id << ")");
      std::map<unsigned int, time::timeperiod::ptr>::iterator found_excluded =
        timeperiods.find(timeperiod_exclude_id);
      if (found_excluded == timeperiods.end())
        throw (reader_exception()
               << "BAM: found a timeperiod exclude pointing to an inexisting "
                  "excluded timeperiod (excluded timeperiod has ID "
               << timeperiod_exclude_id << ")");
      found->second->add_excluded(found_excluded->second);
      misc::shared_ptr<dimension_timeperiod_exclusion>
        exclusion(new dimension_timeperiod_exclusion);
      exclusion->timeperiod_id = timeperiod_id;
      exclusion->excluded_timeperiod_id = timeperiod_exclude_id;
      datas.push_back(exclusion.staticCast<io::data>());
    }

    // Load the BAs.
    q.run_query(
      "SELECT ba_id, name, description,"
      "       sla_month_percent_warn, sla_month_percent_crit,"
      "       sla_month_duration_warn, sla_month_duration_crit,"
      "       id_reporting_period"
      "  FROM cfg_bam"
      "  WHERE activate='1'",
      "could not retrieve BAs from the database");
    while (q.next()) {
      misc::shared_ptr<dimension_ba_event> ba(new dimension_ba_event);
      ba->ba_id = q.value(0).toUInt();
      ba->ba_name = q.value(1).toString();
      ba->ba_description = q.value(2).toString();
      ba->sla_month_percent_warn = q.value(3).toDouble();
      ba->sla_month_percent_crit = q.value(4).toDouble();
      ba->sla_duration_warn = q.value(5).toInt();
      ba->sla_duration_crit = q.value(6).toInt();
      datas.push_back(ba.staticCast<io::data>());
      bas[ba->ba_id] = ba;
      if (!q.value(7).isNull()) {
        misc::shared_ptr<dimension_ba_timeperiod_relation>
          dbtr(new dimension_ba_timeperiod_relation);
        dbtr->ba_id = q.value(0).toUInt();
        dbtr->timeperiod_id = q.value(7).toUInt();
        dbtr->is_default = true;
        datas.push_back(dbtr);
      }
    }

    // Load the BVs.
    q.run_query(
      "SELECT id_ba_group, ba_group_name, ba_group_description"
      "  FROM cfg_bam_ba_groups",
      "could not retrieve BVs from the database");
    while (q.next()) {
      misc::shared_ptr<dimension_bv_event>
          bv(new dimension_bv_event);
      bv->bv_id = q.value(0).toUInt();
      bv->bv_name = q.value(1).toString();
      bv->bv_description = q.value(2).toString();
      datas.push_back(bv.staticCast<io::data>());
    }

    // Load the BA BV relations.
    q.run_query(
      "SELECT id_ba, id_ba_group"
      "  FROM cfg_bam_bagroup_ba_relation as r"
      "  LEFT JOIN cfg_bam as b"
      "    ON b.ba_id = r.id_ba"
      "  WHERE b.activate='1'",
      "could not retrieve BV memberships of BAs");
    while (q.next()) {
      misc::shared_ptr<dimension_ba_bv_relation_event>
          babv(new dimension_ba_bv_relation_event);
      babv->ba_id = q.value(0).toUInt();
      babv->bv_id = q.value(1).toUInt();
      datas.push_back(babv.staticCast<io::data>());
    }

    // Load the KPIs
    // Unfortunately, we need to get the names of the
    // service/host/meta_service/ba/boolean expression associated with
    // this KPI. This explains the numerous joins.
    q.run_query(
      "SELECT k.kpi_id, k.kpi_type, k.host_id, k.service_id, k.id_ba,"
      "       k.id_indicator_ba, k.meta_id, k.boolean_id,"
      "       COALESCE(COALESCE(k.drop_warning, ww.impact), g.average_impact),"
      "       COALESCE(COALESCE(k.drop_critical, cc.impact), g.average_impact),"
      "       COALESCE(COALESCE(k.drop_unknown, uu.impact), g.average_impact),"
      "       h.host_name, s.service_description, b.name,"
      "       meta.meta_name, boo.name"
      "  FROM cfg_bam_kpi AS k"
      "  LEFT JOIN cfg_bam_impacts AS ww"
      "    ON k.drop_warning_impact_id = ww.id_impact"
      "  LEFT JOIN cfg_bam_impacts AS cc"
      "    ON k.drop_critical_impact_id = cc.id_impact"
      "  LEFT JOIN cfg_bam_impacts AS uu"
      "    ON k.drop_unknown_impact_id = uu.id_impact"
      "  LEFT JOIN cfg_hosts AS h"
      "    ON h.host_id = k.host_id"
      "  LEFT JOIN cfg_services AS s"
      "    ON s.service_id = k.service_id"
      "  INNER JOIN cfg_bam AS b"
      "    ON b.ba_id = k.id_ba"
      "  LEFT JOIN cfg_meta_services AS meta"
      "    ON meta.meta_id = k.meta_id"
      "  LEFT JOIN cfg_bam_boolean as boo"
      "    ON boo.boolean_id = k.boolean_id"
      "  LEFT JOIN (SELECT id_ba, 100.0 / COUNT(kpi_id) AS average_impact"
      "               FROM cfg_bam_kpi"
      "               WHERE activate='1'"
      "               GROUP BY id_ba) AS g"
      "   ON k.id_ba=g.id_ba"
      "  WHERE k.activate='1'"
      "    AND b.activate='1'",
      "could not retrieve KPI dimensions");
    while (q.next()) {
      misc::shared_ptr<dimension_kpi_event> k(new dimension_kpi_event);
      k->kpi_id = q.value(0).toUInt();
      k->host_id = q.value(2).toUInt();
      k->service_id = q.value(3).toUInt();
      k->ba_id = q.value(4).toUInt();
      k->kpi_ba_id = q.value(5).toUInt();
      k->meta_service_id = q.value(6).toUInt();
      k->boolean_id = q.value(7).toUInt();
      k->impact_warning = q.value(8).toDouble();
      k->impact_critical = q.value(9).toDouble();
      k->impact_unknown = q.value(10).toDouble();
      k->host_name = q.value(11).toString();
      k->service_description = q.value(12).toString();
      k->ba_name = q.value(13).toString();
      k->meta_service_name = q.value(14).toString();
      k->boolean_name = q.value(15).toString();

      // Resolve the id_indicator_ba.
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

    // Load the ba-timeperiods relations.
    q.run_query(
      "SELECT ba_id, tp_id FROM cfg_bam_relations_ba_timeperiods",
      "could not retrieve the timeperiods associated with the BAs");
    while (q.next()) {
      misc::shared_ptr<dimension_ba_timeperiod_relation>
        dbtr(new dimension_ba_timeperiod_relation);
      dbtr->ba_id = q.value(0).toUInt();
      dbtr->timeperiod_id = q.value(1).toUInt();
      dbtr->is_default = false;
      datas.push_back(dbtr);
    }

    // End the update.
    dtts = misc::shared_ptr<dimension_truncate_table_signal>(
             new dimension_truncate_table_signal);
    dtts->update_started = false;
    datas.push_back(dtts);

    // Write all the cached data to the publisher.
    for (std::vector<misc::shared_ptr<io::data> >::iterator
           it(datas.begin()),
           end(datas.end());
         it != end;
         ++it)
      out->write(*it);
  }
  catch (reader_exception const& e) {
    (void)e;
    throw ;
  }
  catch (std::exception const& e) {
    throw (reader_exception()
           << "BAM: could not load some dimension table: " << e.what());
  }

  return ;
}
