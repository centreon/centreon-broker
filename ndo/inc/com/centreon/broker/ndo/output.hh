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

#ifndef CCB_NDO_OUTPUT_HH
#  define CCB_NDO_OUTPUT_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                ndo {
  /**
   *  @class output output.hh "com/centreon/broker/ndo/output.hh"
   *  @brief NDO output destination.
   *
   *  The class converts events to an output stream using a modified
   *  version of the NDO protocol.
   */
  class                  output : virtual public io::stream {
  public:
                         output();
                         output(output const& o);
    virtual              ~output();
    output&              operator=(output const& o);
    void                 process(bool in = false, bool out = false);
    virtual void         read(misc::shared_ptr<io::data>& e);
    virtual void         statistics(io::properties& tree) const;
    virtual unsigned int write(misc::shared_ptr<io::data> const& e);

  private:
    bool                 _process_out;
  };
}

CCB_END()

#endif // !CCB_NDO_OUTPUT_HH
