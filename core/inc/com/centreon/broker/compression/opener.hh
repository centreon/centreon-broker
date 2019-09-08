/*
** Copyright 2011 Centreon
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

#ifndef CCB_COMPRESSION_OPENER_HH
#define CCB_COMPRESSION_OPENER_HH

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace compression {
/**
 *  @class opener opener.hh "com/centreon/broker/compression/opener.hh"
 *  @brief Open a compression stream.
 *
 *  Open a compression stream.
 */
class opener : public io::endpoint {
 public:
  opener();
  opener(opener const& o);
  ~opener();
  opener& operator=(opener const& o);
  std::shared_ptr<io::stream> open();
  void set_level(int level = -1);
  void set_size(unsigned int size = 0);

 private:
  std::shared_ptr<io::stream> _open(std::shared_ptr<io::stream> stream);

  int _level;
  unsigned int _size;
};
}  // namespace compression

CCB_END()

#endif  // !CCB_COMPRESSION_OPENER_HH
