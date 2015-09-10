/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_NDO_INPUT_HH
#  define CCB_NDO_INPUT_HH

#  include <sstream>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                ndo {
  /**
   *  @class input input.hh "com/centreon/broker/ndo/input.hh"
   *  @brief NDO input source.
   *
   *  The class converts an input stream into events using a modified
   *  version of the NDO protocol.
   */
  class                  input : virtual public io::stream {
  public:
                         input();
                         input(input const& i);
    virtual              ~input();
    input&               operator=(input const& i);
    void                 process(bool in = false, bool out = false);
    virtual void         read(misc::shared_ptr<io::data>& d);
    virtual unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
    char const*          _get_line();
    template             <typename T>
    T*                   _handle_event();

    std::string          _buffer;
    size_t               _old;
    bool                 _process_in;
  };
}

CCB_END()

#endif // !CCB_NDO_INPUT_HH
