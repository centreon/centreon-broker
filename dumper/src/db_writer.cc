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

#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_preparator.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/db_writer.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"

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
      }
    }
    else if (d->type() == entries::ba::static_type()) {
      entries::ba const& b(d.ref_as<entries::ba const>());
      if (b.poller_id == config::applier::state::instance().poller_id())
        _bas.push_back(b);
    }
    else if (d->type() == entries::kpi::static_type()) {
      entries::kpi const& k(d.ref_as<entries::kpi const>());
      if (k.poller_id == config::applier::state::instance().poller_id())
        _kpis.push_back(k);
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
      q.run_query("DELETE cfg_bam_kpi");
    }
    {
      database_query q(db);
      q.run_query("DELETE cfg_bam");
    }
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

  // Process all BAs.
  for (std::list<entries::ba>::const_iterator it(_bas.begin()), end(_bas.end());
       it != end;
       ++it) {
    // INSERT / UPDATE.
    if (it->enable) {
      ba_update << *it;
      ba_update.run_statement();
      if (!ba_update.num_rows_affected()) {
        ba_insert << *it;
        ba_insert.run_statement();
      }
    }
    // DELETE.
    else {
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
      kpi_update << *it;
      kpi_update.run_statement();
      if (!kpi_update.num_rows_affected()) {
        kpi_insert << *it;
        kpi_insert.run_statement();
      }
    }
    // DELETE.
    else {
      kpi_delete.bind_value(":kpi_id", it->kpi_id);
      kpi_delete.run_statement();
    }
  }

  return ;
}
