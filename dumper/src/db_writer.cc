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
#include "com/centreon/broker/dumper/db_writer.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"

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
        _ba_types.clear();
        _bas.clear();
        _kpis.clear();
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

  // Clean database if necessary.
  if (_full_dump) {
    {
      database_query q(db);
      q.run_query("DELETE FROM cfg_bam_kpi");
    }
    {
      database_query q(db);
      q.run_query("DELETE FROM cfg_bam");
    }
    {
      database_query q(db);
      q.run_query("DELETE FROM cfg_bam_ba_types");
    }
  }

  // Prepare organization queries.
  database_query organization_insert(db);
  database_query organization_update(db);
  database_query organization_delete(db);
  {
    database_preparator::event_unique ids;
    ids.insert("organization_id");
    database_preparator dbp(entries::organization::static_type(), ids);
    dbp.prepare_insert(organization_insert);
    dbp.prepare_update(organization_update);
    dbp.prepare_delete(organization_delete);
  }

  // Prepare BA type queries.
  database_query ba_type_insert(db);
  database_query ba_type_update(db);
  database_query ba_type_delete(db);
  {
    database_preparator::event_unique ids;
    ids.insert("ba_type_id");
    database_preparator dbp(entries::ba_type::static_type(), ids);
    dbp.prepare_insert(ba_type_insert);
    dbp.prepare_update(ba_type_update);
    dbp.prepare_delete(ba_type_delete);
  }

  // Prepare BA queries.
  database_query ba_insert(db);
  database_query ba_update(db);
  database_query ba_delete(db);
  {
    database_preparator::event_unique ids;
    ids.insert("ba_id");
    database_preparator dbp(entries::ba::static_type(), ids);
    dbp.prepare_insert(ba_insert);
    dbp.prepare_update(ba_update);
    dbp.prepare_delete(ba_delete);
  }

  // Prepare KPI queries.
  database_query kpi_insert(db);
  database_query kpi_update(db);
  database_query kpi_delete(db);
  {
    database_preparator::event_unique ids;
    ids.insert("kpi_id");
    database_preparator dbp(entries::kpi::static_type(), ids);
    dbp.prepare_insert(kpi_insert);
    dbp.prepare_update(kpi_update);
    dbp.prepare_delete(kpi_delete);
  }

  // Process all organizations.
  for (std::list<entries::organization>::const_iterator
         it(_organizations.begin()),
         end(_organizations.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      logging::debug(logging::medium)
        << "db_dmper: updating organization " << it->organization_id
        << " ('" << it->name << "')";
      organization_update << *it;
      organization_update.run_statement();
      if (!organization_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting organization " << it->organization_id
          << " ('" << it->name << "')";
        organization_insert << *it;
        organization_insert.run_statement();
      }
    }
    // DELETE
    else {
      logging::debug(logging::medium)
        << "db_dumper: deleting organization " << it->organization_id
        << " ('" << it->name << "')";
      organization_delete.bind_value(
                            ":organization_id",
                            it->organization_id);
      organization_delete.run_statement();
    }
  }

  // Process all BA types.
  for (std::list<entries::ba_type>::const_iterator
         it(_ba_types.begin()),
         end(_ba_types.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      logging::debug(logging::medium) << "db_dumper: updating BA type "
        << it->ba_type_id << " ('" << it->name << "')";
      ba_type_update << *it;
      ba_type_update.run_statement();
      if (!ba_type_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting BA type " << it->ba_type_id << " ('"
          << it->name << "')";
        ba_type_insert << *it;
        ba_type_insert.run_statement();
      }
    }
    // DELETE.
    else {
      logging::debug(logging::medium) << "db_dumper: deleting BA type "
        << it->ba_type_id << " ('" << it->name << "')";
      ba_type_delete.bind_value(":ba_type_id", it->ba_type_id);
      ba_type_delete.run_statement();
    }
  }

  // Process all BAs.
  for (std::list<entries::ba>::const_iterator it(_bas.begin()), end(_bas.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      logging::debug(logging::medium)
        << "db_dumper: updating BA " << it->ba_id << " ('" << it->name
        << "')";
      ba_update << *it;
      ba_update.run_statement();
      if (!ba_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting BA " << it->ba_id << " ('"
          << it->name << "')";
        ba_insert << *it;
        ba_insert.run_statement();
      }
      std::ostringstream query;
      query << "UPDATE cfg_bam SET activate='1' WHERE ba_id="
            << it->ba_id;
      database_query q(db);
      q.run_query(query.str().c_str());
    }
    // DELETE.
    else {
      logging::debug(logging::medium)
        << "db_dumper: deleting BA " << it->ba_id << " ('"
        << it->name << "')";
      ba_delete.bind_value(":ba_id", it->ba_id);
      ba_delete.run_statement();
    }
  }

  // Process all KPIs.
  for (std::list<entries::kpi>::const_iterator
         it(_kpis.begin()),
         end(_kpis.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      logging::debug(logging::medium)
        << "db_dumper: updating KPI " << it->kpi_id;
      kpi_update << *it;
      kpi_update.run_statement();
      if (!kpi_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting KPI " << it->kpi_id;
        kpi_insert << *it;
        kpi_insert.run_statement();
      }
      std::ostringstream query;
      query << "UPDATE cfg_bam_kpi SET activate='1' WHERE kpi_id="
            << it->kpi_id;
      database_query q(db);
      q.run_query(query.str().c_str());
    }
    // DELETE.
    else {
      logging::debug(logging::medium)
        << "db_dumper: deleting KPI " << it->kpi_id;
      kpi_delete.bind_value(":kpi_id", it->kpi_id);
      kpi_delete.run_statement();
    }
  }

  return ;
}
