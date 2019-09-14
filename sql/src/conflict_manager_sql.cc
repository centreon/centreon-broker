/*
** Copyright 2019 Centreon
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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"
#include "com/centreon/broker/query_preparator.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 */
void conflict_manager::_process_instances() {

  uint16_t elem{static_cast<uint16_t>(neb::instance::static_type())};
  for (std::list<std::pair<std::shared_ptr<io::data>, bool*> >::iterator
           it(_neb_events[elem].begin()),
       end(_neb_events[elem].end());
       it != end;
       ++it) {
    neb::instance& i(*static_cast<neb::instance*>(it->first.get()));

    // Log message.
    logging::info(logging::medium) << "SQL: processing poller event "
                                   << "(id: " << i.poller_id
                                   << ", name: " << i.name << ", running: "
                                   << (i.is_running ? "yes" : "no") << ")";

    if (i.is_running) {
      // Clean tables.
      _clean_tables(i.poller_id);
    }

    // Processing.
    if (_is_valid_poller(i.poller_id)) {
      // Prepare queries.
      if (!_instance_insupdate.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("instance_id");
        query_preparator qp(neb::instance::static_type(), unique);
        _instance_insupdate = qp.prepare_insert_or_update(_mysql);
      }

      // Process object.
      std::ostringstream oss;
      oss << "SQL: could not store poller (poller: " << i.poller_id << "): ";
      _instance_insupdate << i;

      _mysql.run_statement(_instance_insupdate,
                            oss.str(),
                            true,
                            _mysql.choose_connection_by_instance(i.poller_id));
    }
  }
  /* All the stuff on instances are done, we commit. */
  _mysql.commit();

  /* We just have to set the booleans */
  _set_booleans(_neb_events[static_cast<uint16_t>(neb::instance::static_type())]);
}

void conflict_manager::_clean_tables(uint32_t instance_id) {}

bool conflict_manager::_is_valid_poller(uint32_t instance_id) {
  return true;
}
