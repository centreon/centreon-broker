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

#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/db_reader.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/diff.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/state.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Send a copy of a set of objects.
 *
 */
template <typename T>
static void send_objects(std::list<T> const& t) {
  multiplexing::publisher pblshr;
  for (typename std::list<T>::const_iterator
         it(t.begin()),
         end(t.end());
       it != end;
       ++it) {
    misc::shared_ptr<T> e(new T(*it));
    pblshr.write(e);
  }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] name    Name of endpoint.
 *  @param[in] db_cfg  Database configuration.
 */
db_reader::db_reader(
             std::string const& name,
             database_config const& db_cfg)
  : _loader(db_cfg), _name(name) {}

/**
 *  Destructor.
 */
db_reader::~db_reader() {}

/**
 *  Throws an io::shutdown exception.
 *
 *  @param[in] d         Unused.
 *  @param[in] deadline  Unused.
 *
 *  @return This method will throw.
 */
bool db_reader::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)d;
  (void)deadline;
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from DB configuration reader");
  return (false);
}

/**
 *  Receive external commands.
 *
 *  @param[in] d  Event. Only external command events are processed.
 *
 *  @return Always return 1.
 */
unsigned int db_reader::write(misc::shared_ptr<io::data> const& d) {
  // Process only external commands.
  if (!d.isNull()
      && (d->type() == extcmd::command_request::static_type())) {
    extcmd::command_request const&
      extcmd(d.ref_as<extcmd::command_request const>());
    std::string cmd(extcmd.cmd.toStdString());

    // Discard timestamp.
    size_t start(cmd.find_first_of(' '));
    if (start == std::string::npos) {
      logging::info(logging::medium)
        << "db_reader: ignored improperly formatted external command '"
        << extcmd.cmd << "'";
    }
    else {
      // Split command for processing.
      std::vector<std::string> params;
      misc::string::split(cmd.substr(start + 1), params, ';');
      if (params.size() != 3) {
        logging::info(logging::medium)
          << "db_reader: ignored improperly formatted external command '"
          << extcmd.cmd << "'";
      }
      else {
        // Check that command address ourselves.
        if (params[1] != _name) {
          logging::info(logging::medium)
            << "db_reader: ignored external command '" << params[0]
            << "' that was not intended for ourselves";
        }
        else {
          // Process external commands.
          unsigned int poller_id(strtoul(params[2].c_str(), NULL, 0));
          if (params[0] == "UPDATE_CFG_DB")
            _update_cfg_db(poller_id);
          else if (params[0] == "SYNC_CFG_DB")
            _sync_cfg_db(poller_id);
          else {
            logging::info(logging::medium)
              << "db_reader: unknown external command '"
              << params[0] << "'";
          }
        }
      }
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
 *  Fully synchronize a configuration database.
 *
 *  @param[in] poller_id  Poller ID.
 */
void db_reader::_sync_cfg_db(unsigned int poller_id) {
  if (poller_id) {
    // Log message.
    logging::info(logging::medium)
      << "db_reader: reading a full DB configuration set for poller "
      << poller_id;

    // Discard events of internal cache.
    _cache.erase(poller_id);

    // Read database.
    entries::state state;
    _loader.load(state, poller_id);

    // Send events.
    multiplexing::publisher pblshr;
    {
      misc::shared_ptr<db_dump> start(new db_dump);
      start->full = true;
      start->commit = false;
      start->poller_id = poller_id;
      pblshr.write(start);
    }
    send_objects(state.get_bas());
    send_objects(state.get_kpis());
    {
      misc::shared_ptr<db_dump> end(new db_dump);
      end->full = true;
      end->commit = true;
      end->poller_id = poller_id;
      pblshr.write(end);
    }

    // Update internal cache.
    _cache[poller_id] = state;
  }
  return ;
}

/**
 *  Update (diff) a configuration database.
 *
 *  @param[in] poller_id  Poller ID.
 */
void db_reader::_update_cfg_db(unsigned int poller_id) {
  if (poller_id) {
    // Log message.
    logging::info(logging::medium)
      << "db_reader: reading a partial DB configuration set for poller "
      << poller_id;

    // Read database.
    entries::state state;
    _loader.load(state, poller_id);

    // Diff with existing configuration.
    entries::diff d(_cache[poller_id], state);

    // Send events.
    multiplexing::publisher pblshr;
    {
      misc::shared_ptr<db_dump> start(new db_dump);
      start->full = false;
      start->commit = false;
      start->poller_id = poller_id;
      pblshr.write(start);
    }
    send_objects(d.bas_to_delete());
    send_objects(d.bas_to_update());
    send_objects(d.bas_to_create());
    send_objects(d.kpis_to_delete());
    send_objects(d.kpis_to_update());
    send_objects(d.kpis_to_create());
    {
      misc::shared_ptr<db_dump> end(new db_dump);
      end->full = false;
      end->commit = true;
      end->poller_id = poller_id;
      pblshr.write(end);
    }

    // Update internal cache.
    _cache[poller_id] = state;
  }
  return ;
}
