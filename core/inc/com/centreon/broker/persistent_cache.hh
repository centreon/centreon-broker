/*
** Copyright 2014-2015 Merethis
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

#ifndef CCB_PERSISTENT_CACHE_HH
#  define CCB_PERSISTENT_CACHE_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class persistent_cache persistent_cache.hh "com/centreon/broker/persistent_cache.hh"
 *  @brief Provide a generic persistent cache mechanism.
 *
 *  Provide a generic persistent cache used by some modules to store
 *  data on disk between restarts.
 */
class               persistent_cache {
public:
                    persistent_cache(std::string const& cache_file);
                    ~persistent_cache();
  void              add(misc::shared_ptr<io::data> const& d);
  void              commit();
  void              get(misc::shared_ptr<io::data>& d);
  void              rollback();
  void              transaction();

  std::string const&
                    get_cache_file() const;

private:
                    persistent_cache(persistent_cache const& other);
  persistent_cache& operator=(persistent_cache const& other);
  std::string       _new_file() const;
  std::string       _old_file() const;
  void              _open();

  std::string       _cache_file;
  misc::shared_ptr<io::stream>
                    _read_file;
  misc::shared_ptr<io::stream>
                    _write_file;
};

CCB_END()

#endif // !CCB_PERSISTENT_CACHE_HH
