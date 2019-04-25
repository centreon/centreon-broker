/*
** Copyright 2015,2017 Centreon
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
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/db_dump_committed.hh"
#include "com/centreon/broker/dumper/db_writer.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;
using namespace com::centreon::broker::database;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
db_writer::db_writer(database_config const& db_cfg)
  : _db_cfg(db_cfg), _full_dump(false) {}

/**
 *  Destructor.
 */
db_writer::~db_writer() {}

/**
 *  db_writer cannot read. This method will throw.
 *
 *  @param[in] d         Unused.
 *  @param[in] deadline  Unused.
 *
 *  @return This method will throw an exceptions::shutdown
 *          exception.
 */
bool db_writer::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)d;
  (void)deadline;
  throw (exceptions::shutdown()
         << "cannot read from database configuration writer");
  return (true);
}

/**
 *  Write events.
 *
 *  @param[in] d  Event to write.
 */
int db_writer::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, "db writer"))
    return (1);

  if (d->type() == db_dump::static_type()) {
    db_dump const& dbd(*std::static_pointer_cast<db_dump const>(d));
    if (dbd.poller_id
        == config::applier::state::instance().poller_id()) {
      if (dbd.commit)
        _commit();
      else
        _full_dump = dbd.full;
      _organizations.clear();
      _ba_types.clear();
      _bas.clear();
      _booleans.clear();
      _kpis.clear();
      _hosts.clear();
      _services.clear();
      multiplexing::publisher pblshr;
      std::shared_ptr<db_dump_committed> ddc(new db_dump_committed);
      ddc->req_id = dbd.req_id;
      pblshr.write(ddc);
    }
  }
  else if (d->type() == entries::service::static_type()) {
    entries::service const& s(*std::static_pointer_cast<entries::service const>(d));
    if (s.poller_id == config::applier::state::instance().poller_id())
      _services.push_back(s);
  }
  else if (d->type() == entries::ba::static_type()) {
    entries::ba const& b(*std::static_pointer_cast<entries::ba const>(d));
    if (b.poller_id == config::applier::state::instance().poller_id())
      _bas.push_back(b);
  }
  else if (d->type() == entries::ba_type::static_type()) {
    entries::ba_type const& b(*std::static_pointer_cast<entries::ba_type const>(d));
    _ba_types.push_back(b);
  }
  else if (d->type() == entries::boolean::static_type()) {
    entries::boolean const& b(*std::static_pointer_cast<entries::boolean const>(d));
    if (b.poller_id == config::applier::state::instance().poller_id())
      _booleans.push_back(b);
  }
  else if (d->type() == entries::host::static_type()) {
    entries::host const& h(*std::static_pointer_cast<entries::host const>(d));
    if (h.poller_id == config::applier::state::instance().poller_id())
      _hosts.push_back(h);
  }
  else if (d->type() == entries::kpi::static_type()) {
    entries::kpi const& k(*std::static_pointer_cast<entries::kpi const>(d));
    if (k.poller_id == config::applier::state::instance().poller_id())
      _kpis.push_back(k);
  }
  else if (d->type() == entries::organization::static_type()) {
    entries::organization const&
      o(*std::static_pointer_cast<entries::organization const>(d));
    _organizations.push_back(o);
  }
  return 1;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Commit configuration to database.
 */
void db_writer::_commit() {
  // Open DB connection.
  mysql ms(_db_cfg);
  bool db_v2(ms.schema_version() == mysql::v2);

  // Clean database if necessary.
  if (_full_dump) {
    static char const* const cleanup_v2[] = {
      "DELETE FROM mod_bam_kpi",
      "DELETE FROM mod_bam",
      NULL
    };
    static char const* const cleanup_v3[] = {
      "DELETE FROM cfg_bam_kpi",
      "DELETE FROM cfg_bam",
      "DELETE FROM cfg_bam_ba_types",
      NULL
    };
    char const* const* cleanup_queries;
    if (db_v2)
      cleanup_queries = cleanup_v2;
    else
      cleanup_queries = cleanup_v3;
    for (int i(0); cleanup_queries[i]; ++i)
      ms.run_query(cleanup_queries[i]);
  }

  // Process organizations.
  if (!db_v2)
    _store_objects(
      ms,
      _organizations,
      "organization_id",
      &entries::organization::organization_id);

  // Process BA types.
  if (!db_v2)
    _store_objects(
      ms,
      _ba_types,
      "ba_type_id",
      &entries::ba_type::ba_type_id);

  // Process BAs.
  {
    // Store BAs in their own table.
    _store_objects(
      ms,
      _bas,
      "ba_id",
      &entries::ba::ba_id);

    // Link BAs to a poller.
    {
      std::ostringstream query;
      query << "INSERT INTO " << (db_v2
                                  ? "mod_bam_poller_relations"
                                  : "cfg_bam_poller_relations")
            << "  (ba_id, poller_id)"
               "  VALUES (:ba_id, :poller_id)";
      mysql_stmt stmt(query.str());
      ms.prepare_statement(stmt);

      for (std::list<entries::ba>::const_iterator
             it(_bas.begin()),
             end(_bas.end());
           it != end;
           ++it)
        if (it->enable) {
          stmt.bind_value_as_u32(":ba_id", it->ba_id);
          stmt.bind_value_as_u32(":poller_id", it->poller_id);
          ms.run_statement(stmt);
        }

      // Enable BAs.
      for (std::list<entries::ba>::const_iterator
             it(_bas.begin()),
             end(_bas.end());
           it != end;
           ++it)
        if (it->enable) {
          std::ostringstream query;
          query << "UPDATE " << (db_v2 ? "mod_bam" : "cfg_bam")
                << "  SET activate='1' WHERE ba_id="
                << it->ba_id;
          ms.run_query(query.str());
        }
    }
  }

  // Process booleans.
  if (db_v2)
    _store_objects(
      ms,
      _booleans,
      "boolean_id",
      &entries::boolean::boolean_id);

  // Process KPIs.
  {
    // Store KPIs in their own table.
    _store_objects(
      ms,
      _kpis,
      "kpi_id",
      &entries::kpi::kpi_id);

    // Enable KPIs.
    {
      for (std::list<entries::kpi>::const_iterator
             it(_kpis.begin()),
             end(_kpis.end());
           it != end;
           ++it)
        if (it->enable) {
          std::ostringstream query;
          query << "UPDATE cfg_bam_kpi SET activate='1' WHERE kpi_id="
                << it->kpi_id;
          ms.run_query(query.str());
        }
    }
  }

  // Process virtual hosts.
  if (db_v2)
    _store_objects(
      ms,
      _hosts,
      "host_id",
      &entries::host::host_id);

  // Process virtual services.
  if (db_v2) {
    // Store services in their own table.
    _store_objects(
      ms,
      _services,
      "service_id",
      &entries::service::service_id);

    // Link services to their host.
    std::ostringstream query;
    query << "INSERT INTO host_service_relation"
             "  (host_host_id, service_service_id)"
             "  VALUES (:host_id, :service_id)";
    mysql_stmt stmt(query.str());
    ms.prepare_statement(stmt);
    for (std::list<entries::service>::const_iterator
           it(_services.begin()),
           end(_services.end());
         it != end;
         ++it)
      if (it->enable) {
        stmt.bind_value_as_i32(":host_id", it->host_id);
        stmt.bind_value_as_i32(":service_id", it->service_id);
        ms.run_statement(stmt);
      }
  }
}

/**
 *  Store a list of objects in the database.
 *
 *  @param[out] db  Database object.
 *  @param[in]  l   List of objects.
 *  @param[in]  id  ID name in table.
 */
template <typename T>
void db_writer::_store_objects(
                  mysql& ms,
                  std::list<T> const& l,
                  char const* id_name,
                  unsigned int (T::* id_member)) {
  // Prepare queries.
  query_preparator::event_unique ids;
  ids.insert(id_name);
  query_preparator dbp(T::static_type(), ids);
  mysql_stmt insert_query(dbp.prepare_insert(ms));
  mysql_stmt update_query(dbp.prepare_update(ms));
  mysql_stmt delete_query(dbp.prepare_delete(ms));

  std::string placeholder;
  placeholder = ":";
  placeholder.append(id_name);

  // Process objects.
  for (typename std::list<T>::const_iterator
         it(l.begin()),
         end(l.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      update_query << *it;
      std::promise<int> promise;
      ms.run_statement_and_get_int(
           update_query,
           &promise, mysql_task::int_type::AFFECTED_ROWS);
      if (promise.get_future().get() == 0) {
        insert_query << *it;
        ms.run_statement(insert_query);
      }
    }
    // DELETE.
    else {
      delete_query.bind_value_as_i32(placeholder.c_str(), (*it).*id_member);
      ms.run_statement(delete_query);
    }
  }
}
