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
unsigned int persistent_file::write(
                                misc::shared_ptr<io::data> const& d) {
  return (_file->write(d));
}
