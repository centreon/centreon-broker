/*
** Copyright 2013-2015 Centreon
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

#include <gtest/gtest.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/time/timezone_locker.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

#ifndef __THROW
#define __THROW
#endif  // !__THROW

// # file.conf format
// preferred_time=%Y-%m-%d %H:%M:%S
// current_time=%Y-%m-%d %H:%M:%S
// ref_time=%Y-%m-%d %H:%M:%S
// weekday=monday 00:00-24:00
// speday=XXX
// exclusion=name....
// timeperiod=testing

using namespace com::centreon::broker;

struct options {
  options() : preferred_time(0), ref_time(0) {}
  std::vector<std::shared_ptr<time::timeperiod> > period;
  time_t current_time;
  time_t preferred_time;
  time_t ref_time;
};

static time_t string_to_time_t(std::string const& data) {
  tm t;
  memset(&t, 0, sizeof(t));
  char* ptr(strptime(data.c_str(), "%Y-%m-%d %H:%M:%S", &t));
  if (!ptr)
    throw exceptions::msg() << "invalid date format";
  t.tm_isdst = -1;  // Not set by strptime().
  time::timezone_locker tzlock((*ptr == ' ') ? ptr + 1 : nullptr);
  return mktime(&t);
}

/**
 *  Parse file configuration.
 *
 *  @param[in]  filename  The configuration file path.
 *  @param[out] opt       Struct to fill.
 */
static void parse_file(char const* filename, options& opt) {
  if (!filename)
    throw exceptions::msg() << "null file name";
  std::ifstream stream(filename);
  if (!stream.is_open())
    throw exceptions::msg() << "could not open file '" << filename << "'";
  std::vector<std::string> range;
  std::vector<std::string> exclude;
  std::shared_ptr<time::timeperiod> current_tp(new time::timeperiod);
  while (stream.good()) {
    std::string line;
    std::getline(stream, line, '\n');
    misc::string::trim(line);
    if (line.empty() || line[0] == '#')
      continue;
    size_t pos(line.find_first_of('='));
    if (pos == std::string::npos)
      throw exceptions::msg() << "parsing of file '" << filename
                              << "' failed because of line: " << line;
    std::string key(line.substr(0, pos));
    std::string value(line.substr(pos + 1));
    if (key == "preferred_time")
      opt.preferred_time = string_to_time_t(value);
    else if (key == "current_time")
      opt.current_time = string_to_time_t(value);
    else if (key == "ref_time")
      opt.ref_time = string_to_time_t(value);
    else if (key == "weekday") {
      static char const* const days[] = {"sunday",    "monday",   "tuesday",
                                         "wednesday", "thursday", "friday",
                                         "saturday"};
      for (size_t i(0); i < sizeof(days) / sizeof(*days); ++i) {
        if (!strncmp(days[i], value.c_str(), strlen(days[i]))) {
          std::string v{value.substr(strlen(days[i]))};
          misc::string::trim(v);
          current_tp->set_timerange(v, i);
        }
      }
    } else if (key == "speday") {
      size_t pos(value.find_first_of(" \t\n"));
      if (pos == std::string::npos)
        throw exceptions::msg()
            << "invalid timeperiod exception format: " << value;
      std::string v{value.substr(pos)};
      misc::string::trim(v);
      current_tp->add_exception(value.substr(0, pos), v);
    } else if (key == "exclusion") {
      for (std::vector<std::shared_ptr<time::timeperiod> >::iterator
               it(opt.period.begin()),
           end(opt.period.end());
           it != end; ++it)
        if ((*it)->get_name() == value) {
          current_tp->add_excluded(*it);
          break;
        }
    } else if (key == "timezone")
      current_tp->set_timezone(value);
    else if (key == "timeperiod") {
      current_tp->set_name(value);
      opt.period.push_back(current_tp);
      current_tp = std::shared_ptr<time::timeperiod>(new time::timeperiod);
    } else
      throw exceptions::msg() << "parsing of file '" << filename
                              << "' failed because of line: " << line;
  }
  if (!opt.preferred_time || !opt.current_time || !opt.ref_time ||
      !opt.period.size())
    throw exceptions::msg() << "invalid configuration file: "
                            << "not all required parameters are set";
}

class BamTime : public ::testing::Test {
 public:
  void SetUp() override {
    config::applier::init();
  }

  void TearDown() override {
    config::applier::deinit();
  }
};

bool checkPeriod(char const* file) {
  try {
    // Parse configuration file.
    options opt;
    parse_file(file, opt);

    // Get next valid time.
    time_t valid;
    valid = opt.period.back()->get_next_valid(
        std::max(opt.preferred_time, opt.current_time));

    // Check against reference time.
    if (valid != opt.ref_time) {
      std::string ref_str(ctime(&opt.ref_time));
      misc::string::trim(ref_str);
      std::string valid_str(ctime(&valid));
      misc::string::trim(valid_str);
      throw exceptions::msg()
          << "next valid time of timeperiod is " << valid_str
          << " but does not match reference time " << ref_str;
    }

    // Success.
    return true;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unknown exception" << std::endl;
  }
  return false;
}
/**
 *  Check that the timeperiods work properly.
 *
 *  @return 0 on success.
 */
TEST_F(BamTime, ExceptionCheckPeriodCalendarDate) {
  ASSERT_TRUE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/after_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/before_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/into_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/into_period_with_exclude_into.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/skip_in_after_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/skip_in_before_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/skip_in_into_period.conf"));

  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "skip_in_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "skip_in_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "skip_in_into_period_with_exclude_into.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/start_end_skip_in_after_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "start_end_skip_in_before_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/start_end_skip_in_into_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_in_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_in_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_in_into_period_with_exclude_into.conf"));

  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "start_end_skip_out_after_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/calendar_date/"
                           "start_end_skip_out_before_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/calendar_date/start_end_skip_out_into_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_out_after_period_with_exclude_after.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_out_after_period_with_exclude_before.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/calendar_date/"
                  "start_end_skip_out_after_period_with_exclude_into.conf"));
}

TEST_F(BamTime, ExceptionMonthDate) {
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_date/day_after_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_date/day_before_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_date/day_into_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_date/"
                           "day_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_date/"
                           "day_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/month_date/day_into_period_with_exclude_into.conf"));
}

TEST_F(BamTime, ExceptionMonthDay) {
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_day/after_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_day/before_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_day/into_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/month_day/into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/month_day/into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/month_day/into_period_with_exclude_into.conf"));
}

TEST_F(BamTime, ExceptionMonthWeekDay) {
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_week_day/4_after_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_week_day/4_before_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_week_day/4_into_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "4_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "4_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "4_into_period_with_exclude_into.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_week_day/last_after_period.conf"));
  ASSERT_FALSE(checkPeriod(
      CENTREON_BROKER_BAM_TEST_PATH
      "/time/cfg/exception/month_week_day/last_before_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/month_week_day/last_into_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "last_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "last_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/month_week_day/"
                           "last_into_period_with_exclude_into.conf"));
}

TEST_F(BamTime, ExceptionWeekDay) {
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/4_after_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/week_day/4_before_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/4_into_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "4_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "4_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "4_into_period_with_exclude_into.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/week_day/last_after_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/week_day/last_before_period.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/exception/week_day/last_into_period.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "last_into_period_with_exclude_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "last_into_period_with_exclude_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exception/week_day/"
                           "last_into_period_with_exclude_into.conf"));
}

TEST_F(BamTime, Exclusion) {
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exclusion/nested_after.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exclusion/nested_before.conf"));
  ASSERT_FALSE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                           "/time/cfg/exclusion/nested_into.conf"));
}

TEST_F(BamTime, WeekDay) {
  ASSERT_TRUE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                          "/time/cfg/week_day/after_period.conf"));
  ASSERT_TRUE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                          "/time/cfg/week_day/before_period.conf"));
  ASSERT_TRUE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                          "/time/cfg/week_day/into_period.conf"));
  ASSERT_TRUE(checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                          "/time/cfg/week_day/into_period_for_next_week.conf"));
  ASSERT_TRUE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/week_day/into_period_with_exclude_after.conf"));
  ASSERT_TRUE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/week_day/into_period_with_exclude_before.conf"));
  ASSERT_FALSE(
      checkPeriod(CENTREON_BROKER_BAM_TEST_PATH
                  "/time/cfg/week_day/into_period_with_exclude_into.conf"));
}
