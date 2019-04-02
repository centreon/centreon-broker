/*
** Copyright 2018 Centreon
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

#ifndef CCB_SIMU_STREAM_HH
#  define CCB_SIMU_STREAM_HH

#  include <memory>
#  include <QVariant>
#  include "com/centreon/broker/io/stream.hh"

CCB_BEGIN()

namespace          simu {

  // Forward declaration.
  class luabinding;

  /**
   *  @class stream stream.hh "com/centreon/broker/simu/stream.hh"
   *  @brief lua stream.
   *
   *  Stream events into lua database.
   */
  class             stream : public io::stream {
  public:
                    stream(
                      std::string const& lua_script,
                      QMap<QString, QVariant> const& conf_params);
                    ~stream();
    bool            read(std::shared_ptr<io::data>& d, time_t deadline);
    int             write(std::shared_ptr<io::data> const& d);

  private:
    stream&         operator=(stream const& other);
                    stream(stream const& other);

    // Access to the Lua interpreter
    luabinding*     _luabinding;
  };
}

CCB_END()

#endif // !CCB_SIMU_STREAM_HH
