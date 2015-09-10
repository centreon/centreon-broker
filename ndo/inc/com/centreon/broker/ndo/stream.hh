/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NDO_STREAM_HH
#  define CCB_NDO_STREAM_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/ndo/input.hh"
#  include "com/centreon/broker/ndo/output.hh"

CCB_BEGIN()

namespace        ndo {
  /**
   *  @class stream stream.hh "com/centreon/broker/ndo/stream.hh"
   *  @brief NDO stream.
   *
   *  The class converts data to neb events back and forth.
   */
  class          stream : public input,
                          public output {
  public:
                 stream();
                 stream(stream const& s);
                 ~stream();
    stream&      operator=(stream const& s);
    void         process(bool in = false, bool out = true);
    void         read(misc::shared_ptr<io::data>& d);
    void         statistics(io::properties& tree) const;
    unsigned int write(misc::shared_ptr<io::data> const& d);
  };
}

CCB_END()

#endif // !CCB_NDO_STREAM_HH
