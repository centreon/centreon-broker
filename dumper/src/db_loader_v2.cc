/*
** Copyright 2015 Centreon
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

#include <sstream>
#include <map>
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/dumper/db_loader_v2.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/state.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db  Database object.
 */
db_loader_v2::db_loader_v2(database& db)
  : _db(db), _poller_id(0), _state(NULL) {}

/**
 *  Destructor.
 */
db_loader_v2::~db_loader_v2() {}

/**
 *  Load configuration from database.
 *
 *  @param[out] state      Configuration state.
 *  @param[in]  poller_id  ID of poller for which we want to load
 *                         configuration.
 */
void db_loader_v2::load(entries::state& state, unsigned int poller_id) {
  // Initialization.
  _state = &state;
  _poller_id = poller_id;

  // Database loading.
  _load_bas();
  _load_booleans();
  _load_kpis();
  _load_hosts();
  _load_services();

  // Cleanup.
  _state = NULL;
  _poller_id = 0;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Load BAs.
 */
void db_loader_v2::_load_bas() {
  std::ostringstream query;
  query << "SELECT b.ba_id, b.name, b.description, b.level_w,"
           "       b.level_c"
           "  FROM mod_bam AS b"
           "  INNER JOIN mod_bam_poller_relations AS pr"
           "    ON b.ba_id=pr.ba_id"
           "  WHERE b.activate='1'"
           "    AND pr.poller_id=" << _poller_id;
  database_query q(_db);
  q.run_query(
      query.str(),
      "db_reader: could not load configuration of BAs from DB");
  while (q.next()) {
    entries::ba b;
    b.enable = true;
    b.poller_id = _poller_id;
    b.ba_id = q.value(0).toUInt();
    b.name = q.value(1).toString().toStdString();
    b.description = q.value(2).toString().toStdString();
    b.level_warning = q.value(3).toDouble();
    b.level_critical = q.value(4).toDouble();
    _state->get_bas().push_back(b);
  }
}

/**
 *  Load boolean rules.
 */
void db_loader_v2::_load_booleans() {
  std::ostringstream query;
  query << "SELECT b.boolean_id, b.name, b.expression, b.bool_state,"
           "       b.comments"
           "  FROM mod_bam_boolean AS b"
           "  LEFT JOIN mod_bam_kpi as kpi"
           "    ON b.boolean_id = kpi.boolean_id"
           "  INNER JOIN mod_bam_poller_relations AS pr"
           "    ON kpi.id_ba=pr.ba_id"
           "  WHERE b.activate='1'"
           "    AND pr.poller_id=" << _poller_id;
  database_query q(_db);
  q.run_query(
      query.str(),
      "db_reader: could not load configuration of boolean rules from DB");
  while (q.next()) {
    entries::boolean b;
    b.enable = true;
    b.poller_id = _poller_id;
    b.boolean_id = q.value(0).toUInt();
    b.name = q.value(1).toString().toStdString();
    b.expression = q.value(2).toString().toStdString();
    b.bool_state = q.value(3).toInt();
    b.comment = q.value(4).toString().toStdString();
    _state->get_booleans().push_back(b);
  }
}

/**
 *  Load KPIs.
 */
void db_loader_v2::_load_kpis() {
  std::ostringstream query;
  query << "SELECT k.kpi_id, k.kpi_type, k.host_id, k.service_id,"
           "       k.id_indicator_ba, k.id_ba, k.meta_id, k.boolean_id,"
           "       COALESCE(k.drop_warning, iw.impact),"
           "       COALESCE(k.drop_critical, ic.impact),"
           "       COALESCE(k.drop_unknown, iu.impact)"
           "  FROM mod_bam_kpi AS k"
           "  INNER JOIN mod_bam_poller_relations AS pr"
           "    ON k.id_ba=pr.ba_id"
           "  LEFT JOIN mod_bam_impacts AS iw"
           "    ON k.drop_warning_impact_id=iw.id_impact"
           "  LEFT JOIN mod_bam_impacts AS ic"
           "    ON k.drop_critical_impact_id=ic.id_impact"
           "  LEFT JOIN mod_bam_impacts AS iu"
           "    ON k.drop_unknown_impact_id=iu.id_impact"
           "  WHERE k.activate='1'"
           "    AND pr.poller_id=" << _poller_id;
  database_query q(_db);
  q.run_query(
      query.str(),
      "db_reader: could not load configuration of KPIs from DB");
  while (q.next()) {
    entries::kpi k;
    k.enable = true;
    k.poller_id = _poller_id;
    k.kpi_id = q.value(0).toUInt();
    k.kpi_type = q.value(1).toInt() + 1;
    k.host_id = q.value(2).toUInt();
    k.service_id = q.value(3).toUInt();
    k.ba_indicator_id = q.value(4).toUInt();
    k.ba_id = q.value(5).toUInt();
    k.meta_id = q.value(6).toUInt();
    k.boolean_id = q.value(7).toUInt();
    k.drop_warning = q.value(8).toDouble();
    k.drop_critical = q.value(9).toDouble();
    k.drop_unknown = q.value(10).toDouble();
    _state->get_kpis().push_back(k);
  }
}

/**
 *  Load hosts.
 */
void db_loader_v2::_load_hosts() {
  std::ostringstream query;
  query << "SELECT h.host_id, h.host_name"
           "  FROM host AS h"
           "  WHERE host_name = '_Module_BAM_" << _poller_id << "'";
  database_query q(_db);
  q.run_query(
      query.str(),
      "db_reader: could not load configuration of hosts from DB");
  if (!q.next())
    throw (exceptions::msg()
           << "db_reader: expected virtual host '_Module_BAM_"
           << _poller_id << "'");
  entries::host h;
  h.enable = true;
  h.poller_id = _poller_id;
  h.host_id = q.value(0).toUInt();
  h.name = q.value(1).toString().toStdString();
  _state->get_hosts().push_back(h);
}

/**
 *  Load services.
 */
void db_loader_v2::_load_services() {
  std::map<unsigned int, entries::ba> bas;

  {
    std::list<entries::ba> const& unsorted_bas = _state->get_bas();
    for (std::list<entries::ba>::const_iterator
           it = unsorted_bas.begin(),
           end = unsorted_bas.end();
         it != end;
         ++it)
      bas[it->ba_id] = *it;
  }

  database_query query(_db);
  query.run_query(
          "SELECT s.service_description,"
          "       hsr.host_host_id, hsr.service_service_id"
          "  FROM service AS s"
          "  INNER JOIN host_service_relation AS hsr"
          "    ON s.service_id=hsr.service_service_id"
          "  WHERE s.service_description LIKE 'ba_%'");
  while (query.next()) {
    unsigned int host_id = query.value(1).toUInt();
    unsigned int service_id = query.value(2).toUInt();
    std::string service_description = query.value(0).toString().toStdString();
    std::string trimmed_description = service_description;
    trimmed_description.erase(0, strlen("ba_"));
    if (!trimmed_description.empty()) {
      bool ok;
      unsigned int ba_id;
      try {
        ba_id = std::stoul(trimmed_description);
        ok = true;
      } catch (std::exception const& e) {
        ok = false;
      }
      if (!ok)
        continue ;
      std::map<unsigned int, entries::ba>::const_iterator
        found = bas.find(ba_id);
      if (found == bas.end())
        continue ;
      entries::service s;
      s.enable =  true;
      s.poller_id = _poller_id;
      s.host_id = host_id;
      s.service_id = service_id;
      s.description = service_description;
      _state->get_services().push_back(s);
    }
  }
}
