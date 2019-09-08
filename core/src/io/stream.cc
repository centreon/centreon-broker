/*
** Copyright 2011-2012,2015,2017 Centreon
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

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
stream::stream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
stream::stream(stream const& other) {
  _substream = other._substream;
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& other) {
  if (this != &other)
    _substream = other._substream;
  return *this;
}

/**
 *  Flush data.
 *
 *  @return Number of events acknowledged. This is 0 by default.
 */
int stream::flush() {
  return 0;
}

/**
 *  Get peer name.
 *
 *  @return Peer name.
 */
std::string stream::peer() const {
  return !_substream ? "(unknown)" : _substream->peer();
}

/**
 *  Set sub-stream.
 *
 *  @param[in,out] substream  Stream on which this stream will read and
 *                            write.
 */
void stream::set_substream(std::shared_ptr<stream> substream) {
  _substream = substream;
}

/**
 *  Generate statistics about the stream.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  (void)tree;
}

/**
 *  Configuration update.
 */
void stream::update() {}

/**
 *  Validate an event.
 *
 *  @param[in] d      The event.
 *  @param[in] error  The prefix of the error message.
 *
 *  @return           True if event is valid.
 */
bool stream::validate(std::shared_ptr<io::data> const& d,
                      std::string const& error) {
  if (!d) {
    logging::error(logging::medium)
        << error
        << ": received a null event. This should never happen. "
           "This is likely a software bug that you should report "
           "to Centreon Broker developers.";
    return false;
  }
  return true;
}
