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
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_preparator.hh"
#include "com/centreon/broker/database_query.hh"
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
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

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
 *  @return This method will throw an io::exceptions::shutdown
 *          exception.
 */
bool db_writer::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)d;
  (void)deadline;
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from database configuration writer");
  return (true);
}

/**
 *  Write events.
 *
 *  @param[in] d  Event to write.
 */
unsigned int db_writer::write(misc::shared_ptr<io::data> const& d) {
  if (!d.isNull()) {
    if (d->type() == db_dump::static_type()) {
      db_dump const& dbd(d.ref_as<db_dump const>());
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
        pblshr.write(new db_dump_committed);
      }
    }
    else if (d->type() == entries::ba::static_type()) {
      entries::ba const& b(d.ref_as<entries::ba const>());
      if (b.poller_id == config::applier::state::instance().poller_id())
        _bas.push_back(b);
    }
    else if (d->type() == entries::ba_type::static_type()) {
      entries::ba_type const& b(d.ref_as<entries::ba_type const>());
      _ba_types.push_back(b);
    }
    else if (d->type() == entries::boolean::static_type()) {
      entries::boolean const& b(d.ref_as<entries::boolean const>());
      if (b.poller_id == config::applier::state::instance().poller_id())
        _booleans.push_back(b);
    }
    else if (d->type() == entries::host::static_type()) {
      entries::host const& h(d.ref_as<entries::host const>());
      if (h.poller_id == config::applier::state::instance().poller_id())
        _hosts.push_back(h);
    }
    else if (d->type() == entries::kpi::static_type()) {
      entries::kpi const& k(d.ref_as<entries::kpi const>());
      if (k.poller_id == config::applier::state::instance().poller_id())
        _kpis.push_back(k);
    }
    else if (d->type() == entries::organization::static_type()) {
      entries::organization const&
        o(d.ref_as<entries::organization const>());
      _organizations.push_back(o);
    }
    else if (d->type() == entries::service::static_type()) {
      entries::service const& s(d.ref_as<entries::service const>());
      if (s.poller_id == config::applier::state::instance().poller_id())
        _services.push_back(s);
    }
  }
  return (1);
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
  database db(_db_cfg);
  bool db_v2(db.schema_version() == database::v2);

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
    database_query q(db);
    for (int i(0); cleanup_queries[i]; ++i)
      q.run_query(cleanup_queries[i]);
  }

  // Process organizations.
  if (!db_v2)
    _store_objects(
      db,
      _organizations,
      "organization_id",
      &entries::organization::organization_id);

  // Process BA types.
  if (!db_v2)
    _store_objects(
      db,
      _ba_types,
      "ba_type_id",
      &entries::ba_type::ba_type_id);

  // Process BAs.
  {
    // Store BAs in their own table.
    _store_objects(
      db,
      _bas,
      "ba_id",
      &entries::ba::ba_id);

    // Link BAs to a poller.
    {
      database_query q(db);
      {
        std::ostringstream query;
        query << "INSERT INTO " << (db_v2
                                    ? "mod_bam_poller_relations"
                                    : "cfg_bam_poller_relations")
              << "  (ba_id, poller_id)"
                 "  VALUES (:ba_id, :poller_id)";
        q.prepare(query.str());
      }
      for (std::list<entries::ba>::const_iterator
             it(_bas.begin()),
             end(_bas.end());
           it != end;
           ++it)
        if (it->enable) {
          q.bind_value(":ba_id", it->ba_id);
          q.bind_value(":poller_id", it->poller_id);
          try {
            q.run_statement();
          }
          catch (std::exception const& e) {
            (void)e;
          }
        }
    }

    // Enable BAs.
    {
      database_query q(db);
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
          q.run_query(query.str().c_str());
        }
    }
  }

  // Process booleans.
  if (db_v2)
    _store_objects(
      db,
      _booleans,
      "boolean_id",
      &entries::boolean::boolean_id);

  // Process KPIs.
  {
    // Store KPIs in their own table.
    _store_objects(
      db,
      _kpis,
      "kpi_id",
      &entries::kpi::kpi_id);

    // Enable KPIs.
    {
      database_query q(db);
      for (std::list<entries::kpi>::const_iterator
             it(_kpis.begin()),
             end(_kpis.end());
           it != end;
           ++it)
        if (it->enable) {
          std::ostringstream query;
          query << "UPDATE cfg_bam_kpi SET activate='1' WHERE kpi_id="
                << it->kpi_id;
          q.run_query(query.str().c_str());
        }
    }
  }

  // Process virtual hosts.
  if (db_v2)
    _store_objects(
      db,
      _hosts,
      "host_id",
      &entries::host::host_id);

  // Process virtual services.
  if (db_v2) {
    // Store services in their own table.
    _store_objects(
      db,
      _services,
      "service_id",
      &entries::service::service_id);

    // Link services to their host.
    database_query q(db);
    {
      std::ostringstream query;
      query << "INSERT INTO host_service_relation"
               "  (host_host_id, service_service_id)"
               "  VALUES (:host_id, :service_id)";
      q.prepare(query.str());
    }
    for (std::list<entries::service>::const_iterator
           it(_services.begin()),
           end(_services.end());
         it != end;
         ++it)
      if (it->enable) {
        q.bind_value(":host_id", it->host_id);
        q.bind_value(":service_id", it->service_id);
        try {
          q.run_statement();
        }
        catch (std::exception const& e) {
          (void)e;
        }
      }
  }

  return ;
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
                  database& db,
                  std::list<T> const& l,
                  char const* id_name,
                  unsigned int (T::* id_member)) {
  // Prepare queries.
  database_query insert_query(db);
  database_query update_query(db);
  database_query delete_query(db);
  {
    database_preparator::event_unique ids;
    ids.insert(id_name);
    database_preparator dbp(T::static_type(), ids);
    dbp.prepare_insert(insert_query);
    dbp.prepare_update(update_query);
    dbp.prepare_delete(delete_query);
  }
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
      update_query.run_statement();
      if (!update_query.num_rows_affected()) {
        insert_query << *it;
        insert_query.run_statement();
      }
    }
    // DELETE.
    else {
      delete_query.bind_value(placeholder.c_str(), (*it).*id_member);
      delete_query.run_statement();
    }
  }

  return ;
}
