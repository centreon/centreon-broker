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

#include <sstream>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/db_writer.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "mapping.hh"
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
 *  Set processing flags.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void db_writer::process(bool in, bool out) {
  (void) in;
  (void) out;
  return ;
}

/**
 *  db_writer cannot read. This method will throw.
 *
 *  @param[in] d         Unused.
 */
void db_writer::read(misc::shared_ptr<io::data>& d) {
  (void)d;
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from database configuration writer");
}

/**
 *  Write events.
 *
 *  @param[in] d  Event to write.
 */
unsigned int db_writer::write(misc::shared_ptr<io::data> const& d) {
  if (!d.isNull()) {
    if (d->type() ==
        io::events::data_type<io::events::dumper, dumper::de_db_dump>::value) {
      db_dump const& dbd(d.ref_as<db_dump const>());
      if (dbd.poller_id
          == config::applier::state::instance().get_instance_id()) {
        if (dbd.commit)
          _commit();
        else
          _full_dump = dbd.full;
        _ba_types.clear();
        _bas.clear();
        _kpis.clear();
      }
    }
    else if (d->type() ==
             io::events::data_type<io::events::dumper, dumper::de_entries_ba>::value) {
      entries::ba const& b(d.ref_as<entries::ba const>());
      if (b.poller_id == config::applier::state::instance().get_instance_id())
        _bas.push_back(b);
    }
    else if (d->type() ==
             io::events::data_type<io::events::dumper, dumper::de_entries_ba_type>::value) {
      entries::ba_type const& b(d.ref_as<entries::ba_type const>());
      _ba_types.push_back(b);
    }
    else if (d->type() ==
             io::events::data_type<io::events::dumper, dumper::de_entries_kpi>::value) {
      entries::kpi const& k(d.ref_as<entries::kpi const>());
      if (k.poller_id == config::applier::state::instance().get_instance_id())
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
      q.run_query("DELETE FROM mod_bam_kpi");
    }
    {
      database_query q(db);
      q.run_query("DELETE FROM mod_bam");
    }
    {
      database_query q(db);
      q.run_query("DELETE FROM mod_bam_ba_types");
    }
  }

  // Prepare BA type queries.
  database_query ba_type_insert(db);
  database_query ba_type_update(db);
  database_query ba_type_delete(db);
  {
    std::set<std::string> ids;
    ids.insert("ba_type_id");
    _prepare_insert<entries::ba_type>(ba_type_insert);
    _prepare_update<entries::ba_type>(ba_type_update, ids);
    _prepare_delete<entries::ba_type>(ba_type_delete, ids);
  }

  // Prepare BA queries.
  database_query ba_insert(db);
  database_query ba_update(db);
  database_query ba_delete(db);
  {
    std::set<std::string> ids;
    ids.insert("ba_id");
    _prepare_insert<entries::ba>(ba_insert);
    _prepare_update<entries::ba>(ba_update, ids);
    _prepare_delete<entries::ba>(ba_delete, ids);
  }

  // Prepare KPI queries.
  database_query kpi_insert(db);
  database_query kpi_update(db);
  database_query kpi_delete(db);
  {
    std::set<std::string> ids;
    ids.insert("kpi_id");
    _prepare_insert<entries::kpi>(kpi_insert);
    _prepare_update<entries::kpi>(kpi_update, ids);
    _prepare_delete<entries::kpi>(kpi_delete, ids);
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
      _fill_query(ba_type_update, *it);
      ba_type_update.run_statement();
      if (!ba_type_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting BA type " << it->ba_type_id << " ('"
          << it->name << "')";
        _fill_query(ba_type_insert, *it);
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
      _fill_query(ba_update, *it);
      ba_update.run_statement();
      if (!ba_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting BA " << it->ba_id << " ('"
          << it->name << "')";
        _fill_query(ba_insert, *it);
        ba_insert.run_statement();
      }
      std::ostringstream query;
      query << "UPDATE mod_bam SET activate='1' WHERE ba_id="
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
      _fill_query(kpi_update, *it);
      kpi_update.run_statement();
      if (!kpi_update.num_rows_affected()) {
        logging::debug(logging::medium)
          << "db_dumper: inserting KPI " << it->kpi_id;
        _fill_query(kpi_insert, *it);
        kpi_insert.run_statement();
      }
      std::ostringstream query;
      query << "UPDATE mod_bam_kpi SET activate='1' WHERE kpi_id="
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

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st  Query object.
 */
template <typename T>
void db_writer::_prepare_insert(database_query& st) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      query.append(mapped_type<T>::members[i].name);
      query.append(", ");
    }

  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      query.append(":");
      query.append(mapped_type<T>::members[i].name);
      query.append(", ");
    }
  query.resize(query.size() - 2);
  query.append(")");

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare insertion query");

  return ;
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st  Query object.
 *  @param[in]  id  List of fields that form an UNIQUE.
 */
template <typename T>
void db_writer::_prepare_update(
                  database_query& st,
                  std::set<std::string> const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      bool found(id.find(mapped_type<T>::members[i].name) !=  id.end());
      if (!found) {
        query.append(mapped_type<T>::members[i].name);
        query.append("= :");
        query.append(mapped_type<T>::members[i].name);
        query.append(", ");
      }
    }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (std::set<std::string>::const_iterator
         it(id.begin()),
         end(id.end());
       it != end;
       ++it) {
    query.append("COALESCE(");
    query.append(*it);
    query.append(", -1)=COALESCE(:");
    query.append(*it);
    query.append(", -1)");
    query.append(" AND ");
  }
  query.resize(query.size() - 5);

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare update query");

  return ;
}

/**
 *  Prepare a deletion query.
 *
 *  @param[out] st  Query object.
 *  @param[in]  id  List of fields that form an UNIQUE.
 */
template <typename T>
void db_writer::_prepare_delete(
                  database_query& st,
                  std::set<std::string> const& id) {
  // Build query string.
  std::string query;
  query = "DELETE FROM ";
  query.append(mapped_type<T>::table);
  query.append(" WHERE ");
  for (std::set<std::string>::const_iterator
         it(id.begin()),
         end(id.end());
       it != end;
       ++it) {
    query.append("COALESCE(");
    query.append(*it);
    query.append(", -1)=COALESCE(:");
    query.append(*it);
    query.append(", -1)");
    query.append(" AND ");
  }
  query.resize(query.size() - 5);

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare deletion query");

  return ;
}

/**
 *  Get a boolean from an object.
 */
static void bind_boolean(
              QString const& field,
              bool val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get a double from an object.
 */
static void bind_double(
              QString const& field,
              double val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer from an object.
 */
static void bind_integer(
              QString const& field,
              int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer that is null on zero.
 */
static void bind_integer_null_on_zero(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an integer that is null on -1.
 */
static void bind_integer_null_on_minus_one(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val != -1)
    q.bind_value(field, QVariant(val));
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get a short from an object.
 */
static void bind_short(
              QString const& field,
              short val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get a string from an object.
 */
static void bind_string(
              QString const& field,
              QString const& val,
              database_query& q) {
  q.bind_value(field, QVariant(val.toStdString().c_str()));
  return ;
}

/**
 *  Get a string that might be null from an object.
 */
static void bind_string_null_on_empty(
              QString const& field,
              QString const& val,
              database_query& q) {
  // Not-NULL.
  if (!val.isEmpty())
    q.bind_value(field, QVariant(val.toStdString().c_str()));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::String));
  return ;
}

/**
 *  Get a time_t from an object.
 */
static void bind_timet(
              QString const& field,
              time_t val,
              database_query& q) {
  q.bind_value(
      field,
      QVariant(static_cast<qlonglong>(val)));
  return ;
}

/**
 *  Get a time_t that is null on 0.
 */
static void bind_timet_null_on_zero(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get a time_t that is null on -1.
 */
static void bind_timet_null_on_minus_one(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall != -1)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get an unsigned int from an object.
 */
static void bind_uint(
              QString const& field,
              unsigned int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an unsigned int that is null on zero.
 */
static void bind_uint_null_on_zero(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an unsigned int that is null on -1.
 */
static void bind_uint_null_on_minus_one(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val != (unsigned int)-1)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Bind an event to a query.
 *
 *  @param[in,out] st     The query.
 *  @param[in]     event  The event.
 */
template <typename T>
void db_writer::_fill_query(
                  database_query& st,
                  T const& event) {
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      QString field(":");
      field.append(mapped_type<T>::members[i].name);
      switch (mapped_type<T>::members[i].type) {
      case mapped_data<T>::BOOL:
        bind_boolean(field, event.*mapped_type<T>::members[i].member.b, st);
        break ;
      case mapped_data<T>::DOUBLE:
        bind_double(field, (event.*mapped_type<T>::members[i].member.d), st);
        break ;
      case mapped_data<T>::INT:
        switch (mapped_type<T>::members[i].null_on_value) {
        case NULL_ON_ZERO:
          bind_integer_null_on_zero(
            field,
           event.*mapped_type<T>::members[i].member.i,
            st);
          break ;
        case NULL_ON_MINUS_ONE:
          bind_integer_null_on_minus_one(
            field,
            event.*mapped_type<T>::members[i].member.i,
            st);
          break ;
        default:
          bind_integer(field, event.*mapped_type<T>::members[i].member.i, st);
        }
        break ;
      case mapped_data<T>::SHORT:
        bind_short(field, event.*mapped_type<T>::members[i].member.s, st);
        break ;
      case mapped_data<T>::STRING:
        if (mapped_type<T>::members[i].null_on_value
            == NULL_ON_ZERO)
          bind_string_null_on_empty(
            field,
            event.*mapped_type<T>::members[i].member.S,
            st);
        else
          bind_string(field, event.*mapped_type<T>::members[i].member.S, st);
        break ;
      case mapped_data<T>::TIMESTAMP:
        switch (mapped_type<T>::members[i].null_on_value) {
        case NULL_ON_ZERO:
          bind_timet_null_on_zero(
            field,
            event.*mapped_type<T>::members[i].member.t,
            st);
          break ;
        case NULL_ON_MINUS_ONE:
          bind_timet_null_on_minus_one(
            field,
            event.*mapped_type<T>::members[i].member.t,
            st);
          break ;
        default:
          bind_timet(field, event.*mapped_type<T>::members[i].member.t, st);
        }
        break ;
      case mapped_data<T>::UINT:
        switch (mapped_type<T>::members[i].null_on_value) {
        case NULL_ON_ZERO:
          bind_uint_null_on_zero(
            field,
            event.*mapped_type<T>::members[i].member.u,
            st);
          break ;
        case NULL_ON_MINUS_ONE:
          bind_uint_null_on_minus_one(
            field,
            event.*mapped_type<T>::members[i].member.u,
            st);
          break ;
        default :
          bind_uint(field, event.*mapped_type<T>::members[i].member.u, st);
        }
        break ;
      }
  }
}
