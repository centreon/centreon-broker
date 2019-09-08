/*
** Copyright 2012,2015 Centreon
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

#ifndef CCB_TEST_CENTENGINE_HH
#define CCB_TEST_CENTENGINE_HH

#include <QProcess>
#include <fstream>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "test/centengine_config.hh"
#include "test/centengine_extcmd.hh"

CCB_BEGIN()

namespace test {
/**
 *  @class centengine centengine.hh "test/centengine.hh"
 *  @brief Monitoring engine.
 *
 *  Some monitoring engine such as Centreon Engine.
 */
class centengine {
 public:
  centengine(centengine_config const* cfg);
  ~centengine();
  centengine_extcmd& extcmd();
  void reload();
  void start();
  void stop();

 private:
  centengine(centengine const& other);
  centengine& operator=(centengine const& other);
  void _write_cfg();
  void _write_objs(std::ofstream& ofs, centengine_config::objlist const& objs);

  centengine_config const* _config;
  std::string _config_path;
  QProcess _engine;
  centengine_extcmd _extcmd;
};
}  // namespace test

CCB_END()

#endif  // !CCB_TEST_CENTENGINE_HH
