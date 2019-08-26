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

#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/db_dump_committed.hh"
#include "com/centreon/broker/dumper/db_loader.hh"
#include "com/centreon/broker/dumper/db_reader.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
#include "com/centreon/broker/dumper/entries/diff.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/dumper/entries/state.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
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
    std::shared_ptr<T> e(new T(*it));
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
  : _db_cfg(db_cfg), _name(name.c_str()) {}

/**
 *  Destructor.
 */
db_reader::~db_reader() {}

/**
 *  Throws a shutdown exception.
 *
 *  @param[in] d         Unused.
 *  @param[in] deadline  Unused.
 *
 *  @return This method will throw.
 */
bool db_reader::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)d;
  (void)deadline;
  throw (exceptions::shutdown()
         << "cannot read from DB configuration reader");
  return false;
}

/**
 *  Receive external commands.
 *
 *  @param[in] d  Event. Only external command events are processed.
 *
 *  @return Always return 1.
 */
int db_reader::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, "db_reader"))
    return 1;

  // Process only external commands addressed to us.
  if (d->type() == extcmd::command_request::static_type()) {
    extcmd::command_request const&
      req(*std::static_pointer_cast<extcmd::command_request const>(d));
    if (req.is_addressed_to(_name)) {
      logging::info(logging::medium)
        << "db_reader: processing command: " << req.cmd;
      // Cache the source id for asynchronuous response.
      _req_id_to_source_id[req.uuid] = req.source_id;
      try {
        // Split command for processing.
        std::vector<std::string> params;
        misc::string::split(req.cmd, params, ';');
        if (params.size() != 2) {
          throw (exceptions::msg()
                 << "invalid format: expected format is"
                 << " <UPDATE_CFG_DB|SYNC_CFG_DB>;<POLLERID>");
        }
        else {
          // Process external commands.
          unsigned int poller_id(strtoul(params[1].c_str(), NULL, 0));
          if (params[0] == "UPDATE_CFG_DB")
            _update_cfg_db(poller_id, req.uuid);
          else if (params[0] == "SYNC_CFG_DB")
            _sync_cfg_db(poller_id, req.uuid);
          else {
            throw (exceptions::msg()
                   << "unknown command: valid commands are"
                   << " UPDATE_CFG and SYNC_CFG_DB");
          }
        }
      }
      catch (std::exception const& e) {
        // Log error.
        logging::error(logging::medium)
          << "db_reader: unable to process command '"
          << req.cmd << "': " << e.what();

        // Send error result.
        std::shared_ptr<extcmd::command_result>
          res(new extcmd::command_result);
        res->uuid = req.uuid;
        res->msg = std::string("\"") + e.what() + "\"";
        res->code = -1;
        res->destination_id = req.source_id;
        multiplexing::publisher().write(res);
      }
    }
  }
  else if (d->type() == dumper::db_dump_committed::static_type()) {
    // Send successful result.
    std::shared_ptr<extcmd::command_result>
      res(new extcmd::command_result);
    if (_req_id_to_source_id.find(
          std::static_pointer_cast<dumper::db_dump_committed>(d)->req_id)
        != _req_id_to_source_id.end()) {
      res->uuid = std::static_pointer_cast<dumper::db_dump_committed>(d)->req_id;
      res->msg = "\"Command successfully executed.\"";
      res->code = 0;
      res->destination_id = _req_id_to_source_id[res->uuid];
      multiplexing::publisher().write(res);
    }
  }
  return 1;
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
void db_reader::_sync_cfg_db(
                  unsigned int poller_id,
                  std::string const& req_id) {
  if (poller_id) {
    // Log message.
    logging::info(logging::medium)
      << "db_reader: reading a full DB configuration set for poller "
      << poller_id;

    // Discard events of internal cache.
    _cache.erase(poller_id);

    // Read database.
    entries::state state;
    db_loader loader(_db_cfg);
    loader.load(state, poller_id);

    // Send events.
    multiplexing::publisher pblshr;
    {
      std::shared_ptr<db_dump> start(new db_dump);
      start->full = true;
      start->commit = false;
      start->poller_id = poller_id;
      start->req_id = req_id;
      pblshr.write(start);
    }
    send_objects(state.get_organizations());
    send_objects(state.get_ba_types());
    send_objects(state.get_bas());
    send_objects(state.get_kpis());
    {
      std::shared_ptr<db_dump> end(new db_dump);
      end->full = true;
      end->commit = true;
      end->poller_id = poller_id;
      end->req_id = req_id;
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
void db_reader::_update_cfg_db(unsigned int poller_id, std::string const& req_id) {
  if (poller_id) {
    // Log message.
    logging::info(logging::medium)
      << "db_reader: reading a partial DB configuration set for poller "
      << poller_id;

    // Read database.
    entries::state state;
    db_loader loader(_db_cfg);
    loader.load(state, poller_id);

    // Diff with existing configuration.
    entries::diff d(_cache[poller_id], state);

    // Send events.
    multiplexing::publisher pblshr;
    {
      std::shared_ptr<db_dump> start(new db_dump);
      start->full = false;
      start->commit = false;
      start->poller_id = poller_id;
      start->req_id = req_id;
      pblshr.write(start);
    }
    send_objects(d.organizations_to_delete());
    send_objects(d.organizations_to_update());
    send_objects(d.organizations_to_create());
    send_objects(d.ba_types_to_delete());
    send_objects(d.ba_types_to_update());
    send_objects(d.ba_types_to_create());
    send_objects(d.bas_to_delete());
    send_objects(d.bas_to_update());
    send_objects(d.bas_to_create());
    send_objects(d.booleans_to_delete());
    send_objects(d.booleans_to_update());
    send_objects(d.booleans_to_create());
    send_objects(d.kpis_to_delete());
    send_objects(d.kpis_to_update());
    send_objects(d.kpis_to_create());
    send_objects(d.hosts_to_delete());
    send_objects(d.hosts_to_update());
    send_objects(d.hosts_to_create());
    send_objects(d.services_to_delete());
    send_objects(d.services_to_update());
    send_objects(d.services_to_create());
    {
      std::shared_ptr<db_dump> end(new db_dump);
      end->full = false;
      end->commit = true;
      end->poller_id = poller_id;
      end->req_id = req_id;
      pblshr.write(end);
    }

    // Update internal cache.
    _cache[poller_id] = state;
  }
  return ;
}
