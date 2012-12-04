/*
** Copyright 2012 Merethis
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

#ifndef TEST_RRD_FILE_HH
#  define TEST_RRD_FILE_HH

#  include <ctime>
#  include <list>
#  include <map>
#  include <string>

/**
 *  @class rrd_file rrd_file.hh "test/rrd_file.hh"
 *  @brief RRD file.
 *
 *  Expose content of a RRD file.
 */
class         rrd_file {
public:
              rrd_file();
              rrd_file(rrd_file const& right);
              ~rrd_file();
  rrd_file&   operator=(rrd_file const& right);
  void        add_data(void const* data, size_t size);
  std::list<std::map<time_t, double> > const&
              get_rras() const throw ();
  void        load(char const* file);

private:
  void        _internal_copy(rrd_file const& right);
  void        _process_line(char const* line);

  std::string _buffer;
  bool        _in_rra;
  std::list<std::map<time_t, double> >
              _rras;
};

#endif // !TEST_RRD_FILE_HH
