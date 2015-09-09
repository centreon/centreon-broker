/*
** Copyright 2013 Centreon
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

#ifndef CCB_BBDO_STREAM_HH
#  define CCB_BBDO_STREAM_HH

#  include "com/centreon/broker/bbdo/input.hh"
#  include "com/centreon/broker/bbdo/output.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           bbdo {
  /**
   *  @class stream stream.hh "com/centreon/broker/bbdo/stream.hh"
   *  @brief BBDO stream.
   *
   *  The class converts data to NEB events back and forth.
   */
  class             stream : public input, public output {
  public:
                    stream(bool is_in, bool is_out);
                    stream(stream const& right);
                    ~stream();
    stream&         operator=(stream const& right);
    void            process(bool in = false, bool out = true);
    void            read(misc::shared_ptr<io::data>& d);
    void            statistics(io::properties& tree) const;
    unsigned int    write(misc::shared_ptr<io::data> const& d);

  private:
    bool            _input_read;
    bool            _output_write;
  };
}

CCB_END()

#endif // !CCB_BBDO_STREAM_HH
