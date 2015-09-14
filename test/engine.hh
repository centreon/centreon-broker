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

#ifndef TEST_ENGINE_HH
#  define TEST_ENGINE_HH

#  include <QProcess>
#  include <string>

/**
 *  @class engine engine.hh "test/engine.hh"
 *  @brief Monitoring engine.
 *
 *  Some Nagios-compatible monitoring engine such as Centreon Engine.
 */
class         engine {
public:
              engine();
              ~engine();
  void        reload();
  void        set_config_file(std::string const& config_file);
  void        start();
  void        stop();

private:
              engine(engine const& right);
  engine&     operator=(engine const& right);

  std::string _config_file;
  QProcess    _engine;
};

#endif // !TEST_ENGINE_HH
