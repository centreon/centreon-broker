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

#ifndef CCB_DUMPER_DB_READER_HH
#define CCB_DUMPER_DB_READER_HH

#include <string>
#include <unordered_map>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/dumper/entries/state.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace dumper {
/**
 *  @class db_reader db_reader.hh "com/centreon/broker/dumper/db_reader.hh"
 *  @brief Dump DB configuration.
 *
 *  Read configuration from database and dumps it.
 */
class db_reader : public io::stream {
 public:
  db_reader(std::string const& name, database_config const& db_cfg);
  ~db_reader();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  int write(std::shared_ptr<io::data> const& d);

 private:
  db_reader(db_reader const& other);
  db_reader& operator=(db_reader const& other);
  void _sync_cfg_db(unsigned int poller_id, std::string const& req_id);
  void _update_cfg_db(unsigned int poller_id, std::string const& req_id);

  std::unordered_map<unsigned int, entries::state> _cache;
  std::unordered_map<std::string, unsigned int> _req_id_to_source_id;
  database_config _db_cfg;
  std::string _name;
};
}  // namespace dumper

CCB_END()

#endif  // !CCB_DUMPER_DB_READER_HH
