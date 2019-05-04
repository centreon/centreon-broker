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
#include "com/centreon/broker/dumper/db_loader_v3.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/dumper/entries/state.hh"
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
db_loader_v3::db_loader_v3(mysql& ms)
  : _mysql(ms), _poller_id(0), _state(NULL) {}

/**
 *  Destructor.
 */
db_loader_v3::~db_loader_v3() {}

/**
 *  Load configuration from database.
 *
 *  @param[out] state      Configuration state.
 *  @param[in]  poller_id  ID of poller for which we want to load
 *                         configuration.
 */
void db_loader_v3::load(entries::state& state, unsigned int poller_id) {
  // Initialization.
  _state = &state;
  _poller_id = poller_id;

  // Database loading.
  _load_organizations();
  _load_ba_types();
  _load_bas();
  _load_kpis();

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
 *  Load BA types.
 */
void db_loader_v3::_load_ba_types() {
  std::promise<database::mysql_result> promise;
  _mysql.run_query_and_get_result(
           "SELECT ba_type_id, name, slug, description"
           "  FROM cfg_bam_ba_types",
           &promise);
  try {
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      entries::ba_type b;
      b.ba_type_id = res.value_as_u32(0);
      b.name = res.value_as_str(1).c_str();
      b.slug = res.value_as_str(2).c_str();
      b.description = res.value_as_str(3).c_str();
      _state->get_ba_types().push_back(b);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "db_reader: could not load BA types from DB"
      << e.what();
  }
}

/**
 *  Load BAs.
 */
void db_loader_v3::_load_bas() {
  std::ostringstream query;
  query << "SELECT b.ba_id, b.name, b.description, b.level_w,"
           "       b.level_c, b.organization_id, b.ba_type_id"
           "  FROM cfg_bam AS b"
           "  INNER JOIN cfg_bam_poller_relations AS pr"
           "    ON b.ba_id=pr.ba_id"
           "  WHERE b.activate='1'"
           "    AND pr.poller_id=" << _poller_id;
  std::promise<database::mysql_result> promise;
  _mysql.run_query_and_get_result(
           query.str(),
           &promise);
  try {
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      entries::ba b;
      b.enable = true;
      b.poller_id = _poller_id;
      b.ba_id = res.value_as_u32(0);
      b.name = res.value_as_str(1).c_str();
      b.description = res.value_as_str(2).c_str();
      b.level_warning = res.value_as_f64(3);
      b.level_critical = res.value_as_f64(4);
      b.organization_id = res.value_as_u32(5);
      b.type_id = res.value_as_u32(6);
      _state->get_bas().push_back(b);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "db_reader: could not load configuration of BAs from DB"
      << e.what();
  }
}

/**
 *  Load KPIs.
 */
void db_loader_v3::_load_kpis() {
  std::ostringstream query;
  query << "SELECT k.kpi_id, k.kpi_type, k.host_id, k.service_id,"
           "       k.id_indicator_ba, k.id_ba, k.meta_id, k.boolean_id,"
           "       COALESCE(k.drop_warning, iw.impact),"
           "       COALESCE(k.drop_critical, ic.impact),"
           "       COALESCE(k.drop_unknown, iu.impact)"
           "  FROM cfg_bam_kpi AS k"
           "  INNER JOIN cfg_bam_poller_relations AS pr"
           "    ON k.id_ba=pr.ba_id"
           "  LEFT JOIN cfg_bam_impacts AS iw"
           "    ON k.drop_warning_impact_id=iw.id_impact"
           "  LEFT JOIN cfg_bam_impacts AS ic"
           "    ON k.drop_critical_impact_id=ic.id_impact"
           "  LEFT JOIN cfg_bam_impacts AS iu"
           "    ON k.drop_unknown_impact_id=iu.id_impact"
           "  WHERE k.activate='1'"
           "    AND pr.poller_id=" << _poller_id;
  std::promise<database::mysql_result> promise;
  _mysql.run_query_and_get_result(
           query.str(),
           &promise);
  try {
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      entries::kpi k;
      k.enable = true;
      k.poller_id = _poller_id;
      k.kpi_id = res.value_as_u32(0);
      k.kpi_type = res.value_as_i32(1) + 1;
      k.host_id = res.value_as_u32(2);
      k.service_id = res.value_as_u32(3);
      k.ba_indicator_id = res.value_as_u32(4);
      k.ba_id = res.value_as_u32(5);
      k.meta_id = res.value_as_u32(6);
      k.boolean_id = res.value_as_u32(7);
      k.drop_warning = res.value_as_f64(8);
      k.drop_critical = res.value_as_f64(9);
      k.drop_unknown = res.value_as_f64(10);
      _state->get_kpis().push_back(k);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "db_reader: could not load configuration of KPIs from DB"
      << e.what();
  }
}

/**
 *  Load organizations.
 */
void db_loader_v3::_load_organizations() {
  std::ostringstream query;
  query << "SELECT o.organization_id, o.name, o.shortname"
           "  FROM cfg_pollers AS p"
           "  INNER JOIN cfg_organizations AS o"
           "    ON p.organization_id=o.organization_id"
           "  WHERE p.poller_id=" << _poller_id;
  std::promise<database::mysql_result> promise;
  _mysql.run_query_and_get_result(
           query.str(),
           &promise);
  try {
    database::mysql_result res(promise.get_future().get());
    if (!_mysql.fetch_row(res))
      throw (exceptions::msg() << "db_reader: poller " << _poller_id
             << " has no organization: cannot load remaining tables");
    entries::organization o;
    o.enable = true;
    o.organization_id = res.value_as_u32(0);
    o.name = res.value_as_str(1).c_str();
    o.shortname = res.value_as_str(2).c_str();
    _state->get_organizations().push_back(o);
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "db_reader: could not load organization from DB"
      << e.what();
  }
}
