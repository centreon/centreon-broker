/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/logging/file.hh"
#include "test/processing/feeder/common.hh"

using namespace com::centreon::broker;

/**
 *  Enable logging on stderr.
 */
void log_on_stderr() {
  logging::file::with_thread_id(true);
  logging::file::with_timestamp(true);
  config::logger log_obj;
  log_obj.config(true);
  log_obj.debug(true);
  log_obj.error(true);
  log_obj.info(true);
  log_obj.level(logging::low);
  log_obj.type(config::logger::standard);
  log_obj.name("stderr");
  QList<config::logger> loggers;
  loggers.push_back(log_obj);
  config::applier::logger::instance().apply(loggers);
  return ;
}
