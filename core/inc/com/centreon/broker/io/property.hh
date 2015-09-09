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

#ifndef CCB_IO_PROPERTY_HH
#  define CCB_IO_PROPERTY_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              io {
  /**
   *  @class property property.hh "com/centreon/broker/io/property.hh"
   *  @brief Single property.
   *
   *  Value and attributes of a single property.
   */
  class                property {
  public:
                       property(
                         std::string const& output = "",
                         std::string const& perfdata = "",
                         bool graphable = false);
                       property(property const& right);
                       ~property() throw ();
    property&          operator=(property const& right);
    std::string const& get_output() const throw ();
    std::string const& get_perfdata() const throw ();
    bool               is_graphable() const throw ();
    void               set_graphable(bool graphable = true);
    void               set_output(std::string const& output);
    void               set_perfdata(std::string const& perfdata);

  private:
    bool               _graphable;
    std::string        _output;
    std::string        _perfdata;
  };
}

CCB_END()

#endif // !CCB_IO_PROPERTY_HH
