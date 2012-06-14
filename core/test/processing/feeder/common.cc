/*
** Copyright 2011-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
