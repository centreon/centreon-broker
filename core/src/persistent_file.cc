/*
** Copyright 2015,2017, 2020-2021 Centreon
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

#include "com/centreon/broker/persistent_file.hh"
#include <memory>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/file/opener.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] path  Path of the persistent file.
 */
persistent_file::persistent_file(const std::string& path)
    : io::stream("persistent_file") {
  // On-disk file.
  file::opener opnr;
  opnr.set_filename(path);
  std::shared_ptr<io::stream> fs(opnr.open());
  _splitter = std::static_pointer_cast<file::stream>(fs);

  // Compression layer.
  std::shared_ptr<compression::stream> cs(
      std::make_shared<compression::stream>());
  cs->set_substream(fs);

  // BBDO layer.
  std::shared_ptr<bbdo::stream> bs(std::make_shared<bbdo::stream>(true));
  bs->set_coarse(true);
  bs->set_negotiate(false);
  bs->set_substream(cs);

  // Set stream.
  io::stream::set_substream(bs);
}

/**
 *  Read data from file.
 *
 *  @param[out] d         Output data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always return true, as file never times out.
 */
bool persistent_file::read(std::shared_ptr<io::data>& d, time_t deadline) {
  return _substream->read(d, deadline);
}

/**
 *  Generate statistics of persistent file.
 *
 *  @param[out] tree  Statistics tree.
 */
void persistent_file::statistics(nlohmann::json& tree) const {
  _substream->statistics(tree);
}

/**
 *  Write data to file.
 *
 *  @param[in] d  Input data.
 */
int32_t persistent_file::write(std::shared_ptr<io::data> const& d) {
  return _substream->write(d);
}

/**
 * @brief Flush the stream and stop it.
 *
 * @return the number of acknowledged events.
 */
int32_t persistent_file::stop() {
  int32_t retval = _substream->stop();
  log_v2::core()->info("persistent file stopped with {} acknowledged events",
                       retval);
  return retval;
}

/**
 *  Remove persistent file.
 */
void persistent_file::remove_all_files() {
  _splitter->remove_all_files();
}
