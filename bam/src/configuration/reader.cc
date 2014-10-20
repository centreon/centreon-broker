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
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_exception.hh"
#include "com/centreon/broker/bam/sql_mapping.hh"

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
    "        COALESCE(k.drop_unknown, uu.impact)"
    "  FROM  mod_bam_kpi AS k"
    "  LEFT JOIN mod_bam_impacts AS ww"
    "    ON k.drop_warning_impact_id = ww.id_impact"
    "  LEFT JOIN mod_bam_impacts AS cc"
    "    ON k.drop_critical_impact_id = cc.id_impact"
    "  LEFT JOIN mod_bam_impacts AS uu"
    "    ON k.drop_unknown_impact_id = uu.id_impact"));
  if (_db->lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve KPI configuration from DB: "
           << _db->lastError().text());

  while (query.next()) {
    kpis.push_back(
      kpi(
        query.value(0).toInt(), // ID.
        query.value(1).toInt(), // State type.
        query.value(2).toInt(), // Host ID.
        query.value(3).toInt(), // Service ID.
        query.value(4).toInt(), // BA ID.
        query.value(5).toInt(), // BA indicator ID.
        query.value(6).toInt(), // Status.
        query.value(7).toInt(), // Hard state.
        query.value(8).toFloat(), // Downtimed.
        query.value(9).toFloat(), // Acknowledged.
        query.value(10).toBool(), // Ignore downtime.
        query.value(11).toBool(), // Ignore acknowledgement.
        query.value(12).toDouble(), // Warning.
        query.value(13).toDouble(), // Critical.
        query.value(14).toDouble())); // Unknown.
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
    "SELECT ba_id, name, level_w, level_c"
    "  FROM mod_bam"));
  if (_db->lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve BA configuration from DB: "
           << _db->lastError().text());

  while (query.next()) {
    bas[query.value(0).toUInt()] =
      ba(
        query.value(0).toUInt(), // ID.
        query.value(1).toString().toStdString(), // Name.
        query.value(2).toFloat(), // Warning level.
        query.value(3).toFloat()); // Critical level.
  }

  // Load the associated ba_id from the table services.
  // All the associated services have for description 'ba_[id]'.
  query = _db->exec("SELECT service_id, service_description"
                    "  FROM services"
                    "  WHERE service_description LIKE 'ba_%'");
  if (_db->lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve BA service ids from DB: "
           << _db->lastError().text());

  while (query.next()) {
    std::string service_description = query.value(1).toString().toStdString();
    service_description.erase(0, strlen("ba_"));
    if (!service_description.empty()) {
      bool ok = false;
      unsigned int ba_id = QString(service_description.c_str()).toUInt(&ok);
      if (!ok)
        continue;
      state::bas::iterator found = bas.find(ba_id);
      if (found == bas.end())
        continue;
      found->second.set_service_id(query.value(0).toUInt());
    }
  }

  // Test for ba without service id.
  for (state::bas::const_iterator it(bas.begin()),
                                  end(bas.end());
       it != end;
       ++it)
    if (it->second.get_service_id() == 0)
      throw (reader_exception()
               << "BAM: found a ba without an associated service, id: "
               << it->second.get_id());
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
    if (_db->lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve boolean expression "
             << "configuration from DB: " << _db->lastError().text());

    while (query.next()) {
      bool_exps.push_back(
                  bool_expression(
                    query.value(0).toInt(), // ID.
                    query.value(1).toFloat(),// Impact.
                    query.value(2).toString().toStdString(), // Expression.
                    query.value(3).toBool())); // Impact if.
    }
  }

  // Load relations of boolean expressions with BAs.
  {
    std::map<unsigned int, bool_expression::ids_of_bas> impacted_bas;
    {
      QSqlQuery q(_db->exec(
        "SELECT boolean_id, ba_id FROM mod_bam_bool_rel"));
      if (_db->lastError().isValid())
        throw (reader_exception()
               << "BAM: could not retrieve BAs impacted by boolean "
               << "expressions: " << _db->lastError().text());

      while (q.next())
        impacted_bas[q.value(0).toUInt()].push_back(q.value(1).toUInt());
    }
    for (state::bool_exps::iterator
           it(bool_exps.begin()), end(bool_exps.end());
         it != end;
         ++it) {
      std::map<unsigned int, bool_expression::ids_of_bas>::iterator
        it_impacted_bas(impacted_bas.find(it->get_id()));
      if (it_impacted_bas != impacted_bas.end())
        it->impacted_bas().swap(it_impacted_bas->second);
      else
        it->impacted_bas().clear();
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
    if (_db->lastError().isValid())
      throw (reader_exception()
             << "BAM: could not retrieve meta-services: "
             << _db->lastError().text());
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
      QSqlQuery q(_db->exec(query.str().c_str()));
      if (_db->lastError().isValid())
        throw (reader_exception()
               << "BAM: could not retrieve members of meta-service '"
               << it->get_name() << "': " << _db->lastError().text());
      while (q.next())
        it->add_metric(q.value(0).toUInt());
    }
    // Service list mode.
    else {
      std::ostringstream query;
      query << "SELECT metric_id"
            << "  FROM meta_service_relation"
            << "  WHERE meta_id=" << it->get_id()
            << "    AND activate='1'";
      QSqlQuery q(_db->exec(query.str().c_str()));
      if (_db->lastError().isValid())
        throw (reader_exception()
               << "BAM: could not retrieve members of meta-service '"
               << it->get_name() << "': " << _db->lastError().text());
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
  QSqlQuery q(_db->exec(
    "SELECT h.host_id, s.service_id, h.name, s.description"
    "  FROM services AS s LEFT JOIN hosts AS h"
    "  ON s.host_id = h.host_id"));
  if (_db->lastError().isValid())
    throw (reader_exception()
           << "BAM: could not retrieve host/service IDs: "
           << _db->lastError().text());
  while (q.next())
    mapping.set_service(
              q.value(2).toString().toStdString(),
              q.value(3).toString().toStdString(),
              q.value(0).toUInt(),
              q.value(1).toUInt());
  return ;
}
