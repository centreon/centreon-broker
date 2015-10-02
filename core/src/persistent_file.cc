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

#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/persistent_file.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] path  Path of the persistent file.
 */
persistent_file::persistent_file(std::string const& path) {
  // On-disk file.
  misc::shared_ptr<file::stream>
    fs(new file::stream(path, 100000000ll));

  // Compression layer.
  misc::shared_ptr<compression::stream> cs(new compression::stream);
  cs->set_substream(fs);

  // BBDO layer.
  misc::shared_ptr<bbdo::stream> bs(new bbdo::stream);
  bs->set_coarse(true);
  bs->set_negociate(false);
  bs->set_substream(cs);

  // Set stream.
  _file = bs;
}

/**
 *  Destructor.
 */
persistent_file::~persistent_file() {}

/**
 *  Read data from file.
 *
 *  @param[out] d         Output data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always return true, as file never times out.
 */
bool persistent_file::read(
                        misc::shared_ptr<io::data>& d,
                        time_t deadline) {
  return (_file->read(d, deadline));
}

/**
 *  Write data to file.
 *
 *  @param[in] d  Input data.
 */
int persistent_file::write(misc::shared_ptr<io::data> const& d) {
  return (_file->write(d));
}
