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

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <QString>
#include <vector>
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/time/timezone_locker.hh"
#include "com/centreon/broker/time/timezone_manager.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"

#ifndef __THROW
#  define __THROW
#endif // !__THROW

// # file.conf format
// preferred_time=%Y-%m-%d %H:%M:%S
// current_time=%Y-%m-%d %H:%M:%S
// ref_time=%Y-%m-%d %H:%M:%S
// weekday=monday 00:00-24:00
// speday=XXX
// exclusion=name....
// timeperiod=testing

using namespace com::centreon::broker;

struct    options {
          options()
  : preferred_time(0), ref_time(0) {}
  std::vector<std::shared_ptr<time::timeperiod> >
          period;
  time_t  current_time;
  time_t  preferred_time;
  time_t  ref_time;
};

static time_t string_to_time_t(std::string const& data) {
  tm t;
  memset(&t, 0, sizeof(t));
  char* ptr(strptime(data.c_str(), "%Y-%m-%d %H:%M:%S", &t));
  if (!ptr)
    throw (exceptions::msg() << "invalid date format");
  t.tm_isdst = -1; // Not set by strptime().
  time::timezone_locker tzlock((*ptr == ' ') ? ptr + 1 : NULL);
  return (mktime(&t));
}

/**
 *  Parse file configuration.
 *
 *  @param[in]  filename  The configuration file path.
 *  @param[out] opt       Struct to fill.
 */
static void parse_file(char const* filename, options& opt) {
  if (!filename)
    throw (exceptions::msg() << "null file name");
  std::ifstream stream(filename);
  if (!stream.is_open())
    throw (exceptions::msg() << "could not open file '"
           << filename << "'");
  std::vector<std::string> range;
  std::vector<std::string> exclude;
  std::shared_ptr<time::timeperiod>
    current_tp(new time::timeperiod);
  while (stream.good()) {
    std::string line;
    std::getline(stream, line, '\n');
    line = QString(line.c_str()).trimmed().toStdString();
    if (line.empty() || line[0] == '#')
      continue ;
    size_t pos(line.find_first_of('='));
    if (pos == std::string::npos)
      throw (exceptions::msg() << "parsing of file '" << filename
             << "' failed because of line: " << line);
    std::string key(line.substr(0, pos));
    std::string value(line.substr(pos + 1));
    if (key == "preferred_time")
      opt.preferred_time = string_to_time_t(value);
    else if (key == "current_time")
      opt.current_time = string_to_time_t(value);
    else if (key == "ref_time")
      opt.ref_time = string_to_time_t(value);
    else if (key == "weekday") {
      static char const* const days[] = {
        "sunday",
        "monday",
        "tuesday",
        "wednesday",
        "thursday",
        "friday",
        "saturday"
      };
      for (size_t i(0); i < sizeof(days) / sizeof(*days); ++i) {
        if (!strncmp(days[i], value.c_str(), strlen(days[i])))
          current_tp->set_timerange(
            QString(value.c_str() + strlen(days[i])).trimmed().toStdString(),
            i);
      }
    }
    else if (key == "speday") {
      size_t pos(value.find_first_of(" \t\n"));
      if (pos == std::string::npos)
        throw (exceptions::msg()
               << "invalid timeperiod exception format: " << value);
      current_tp->add_exception(
                    value.substr(0, pos),
                    QString(value.substr(pos).c_str()).trimmed().toStdString());
    }
    else if (key == "exclusion") {
      for (std::vector<std::shared_ptr<time::timeperiod> >::iterator
             it(opt.period.begin()),
             end(opt.period.end());
           it != end;
           ++it)
        if ((*it)->get_name() == value) {
          current_tp->add_excluded(*it);
          break ;
        }
    }
    else if (key == "timezone")
      current_tp->set_timezone(value);
    else if (key == "timeperiod") {
      current_tp->set_name(value);
      opt.period.push_back(current_tp);
      current_tp = std::shared_ptr<time::timeperiod>(
                     new time::timeperiod);
    }
    else
      throw (exceptions::msg() << "parsing of file '" << filename
             << "' failed because of line: " << line);
  }
  if (!opt.preferred_time
      || !opt.current_time
      || !opt.ref_time
      || !opt.period.size())
    throw (exceptions::msg()
           << "invalid configuration file: "
           << "not all required parameters are set");
}

/**
 *  Check that the timeperiods work properly.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Error flag.
  bool error(true);

  try {
    // Initialization.
    config::applier::init();
    time::timezone_manager::load();

    // Usage.
    if (argc != 2)
      throw (exceptions::msg() << "usage: " << argv[0] << " file.conf");

    // Parse configuration file.
    options opt;
    parse_file(argv[1], opt);

    // Get next valid time.
    time_t valid;
    valid = opt.period.back()->get_next_valid(
                                 std::max(
                                        opt.preferred_time,
                                        opt.current_time));

    // Check against reference time.
    if (valid != opt.ref_time) {
      std::string ref_str(ctime(&opt.ref_time));
      std::string valid_str(ctime(&valid));
      throw (exceptions::msg()
             << "next valid time of timeperiod is "
             << QString(valid_str.c_str()).trimmed()
             << " but does not match reference time "
             << QString(ref_str.c_str()).trimmed());
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  // Deinitialization.
  time::timezone_manager::unload();
  config::applier::deinit();

  // Return exit code.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
