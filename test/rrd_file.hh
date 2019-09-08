/*
** Copyright 2012-2013 Centreon
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

#ifndef TEST_RRD_FILE_HH
#define TEST_RRD_FILE_HH

#include <ctime>
#include <list>
#include <map>
#include <string>

/**
 *  @class rrd_file rrd_file.hh "test/rrd_file.hh"
 *  @brief RRD file.
 *
 *  Expose content of a RRD file.
 */
class rrd_file {
 public:
  rrd_file();
  rrd_file(rrd_file const& right);
  ~rrd_file();
  rrd_file& operator=(rrd_file const& right);
  void add_data(void const* data, size_t size);
  std::list<std::map<time_t, double> > const& get_rras() const throw();
  void load(char const* file);

 private:
  void _internal_copy(rrd_file const& right);
  void _process_line(char const* line);

  std::string _buffer;
  bool _in_rra;
  std::list<std::map<time_t, double> > _rras;
};

#endif  // !TEST_RRD_FILE_HH
