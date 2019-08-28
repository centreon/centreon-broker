/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/notification/builders/timeperiod_builder.hh"
#include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::time;

timeperiod_loader::timeperiod_loader() {

}

/**
 *  Load the timeperiods from the database.
 *
 *  @param[in] ms       An open connection to the database.
 *  @param[out] output   A timeperiod builder object to register the timeperiods.
 */
void timeperiod_loader::load(mysql* ms, timeperiod_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading timeperiods from the database";

  // Performance improvement, as we never go back.
//  query.setForwardOnly(true);

  // Load the timeperiods.
  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result(
        "SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday,"
        "       tp_tuesday, tp_wednesday, tp_thursday, tp_friday,"
        "       tp_saturday sunday"
        " FROM cfg_timeperiods",
        &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      timeperiod::ptr tperiod(new timeperiod);
      unsigned int timeperiod_id = res.value_as_u32(0);
      tperiod->set_name(res.value_as_str(1));
      tperiod->set_alias(res.value_as_str(2));
      tperiod->set_timerange(res.value_as_str(3), 0);
      tperiod->set_timerange(res.value_as_str(4), 1);
      tperiod->set_timerange(res.value_as_str(5), 2);
      tperiod->set_timerange(res.value_as_str(6), 3);
      tperiod->set_timerange(res.value_as_str(7), 4);
      tperiod->set_timerange(res.value_as_str(8), 5);
      tperiod->set_timerange(res.value_as_str(9), 6);

      output->add_timeperiod(
                timeperiod_id,
                tperiod);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load timeperiods from database: "
      << e.what();
  }

  // Load the timeperiod exceptions.
  promise = std::promise<database::mysql_result>();
  ms->run_query_and_get_result(
        "SELECT exception_id, timeperiod_id, days, timerange"
        " FROM cfg_timeperiods_exceptions",
        &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      unsigned int timeperiod_id = res.value_as_u32(1);
      std::string days = res.value_as_str(2);
      std::string timerange = res.value_as_str(3);
      output->add_timeperiod_exception(timeperiod_id, days, timerange);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load timeperiods exceptions from database: "
      << e.what();
  }

  // Load the timeperiod exclude relations.
  promise = std::promise<database::mysql_result>();
  ms->run_query_and_get_result(
        "SELECT exclude_id, timeperiod_id, timeperiod_exclude_id"
        " FROM cfg_timeperiods_exclude_relations",
        &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res))
      output->add_timeperiod_exclude_relation(
                res.value_as_u32(1),
                res.value_as_u32(0));
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load timeperiods exclusions from database: "
      << e.what();
  }

  // Load the timeperiod include relations.
  promise = std::promise<database::mysql_result>();
  ms->run_query_and_get_result(
        "SELECT include_id, timeperiod_id, timeperiod_include_id"
        " FROM cfg_timeperiods_include_relations",
        &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res))
      output->add_timeperiod_include_relation(
                res.value_as_u32(1),
                res.value_as_u32(0));
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load timeperiods inclusions from database: "
      << e.what();
  }
}
