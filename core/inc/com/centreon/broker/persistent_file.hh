/*
** Copyright 2015 Merethis
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

#ifndef CCB_PERSISTENT_FILE_HH
#  define CCB_PERSISTENT_FILE_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class persistent_file persistent_file.hh "com/centreon/broker/persistent_file.hh"
 *  @brief On-disk file.
 *
 *  On-disk file that uses multiple streams to write serialized data.
 *  It uses BBDO, compression and file streams.
 */
class              persistent_file : public io::stream {
public:
                   persistent_file(std::string const& path);
                   ~persistent_file();
  bool             read(
                     misc::shared_ptr<io::data>& d,
                     time_t deadline);
  unsigned int     write(misc::shared_ptr<io::data> const& d);

private:
                   persistent_file(persistent_file const& other);
  persistent_file& operator=(persistent_file const& other);

  misc::shared_ptr<io::stream>
                   _file;
};

CCB_END()

#endif // !CCB_PERSISTENT_FILE_HH
