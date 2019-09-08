/*
** Copyright 2012-2013,2015 Centreon
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

#ifndef TEST_CBD_HH
#define TEST_CBD_HH

#include <QProcess>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace test {
/**
 *  @class cbd cbd.hh "test/cbd.hh"
 *  @brief Centreon Broker daemon.
 *
 *  Centreon Broker daemon.
 */
class cbd {
 public:
  cbd();
  ~cbd();
  void set_config_file(std::string const& config_file);
  void start();
  void stop();
  void update();
  void wait();

 private:
  cbd(cbd const& other);
  cbd& operator=(cbd const& other);

  std::string _config_file;
  QProcess _cbd;
};
}  // namespace test

CCB_END()

#endif  // !TEST_CBD_HH
