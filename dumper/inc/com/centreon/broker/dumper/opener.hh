/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_DUMPER_OPENER_HH
#  define CCB_DUMPER_OPENER_HH

#  include <string>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace                        dumper {
  /**
   *  @class opener opener.hh "com/centreon/broker/dumper/opener.hh"
   *  @brief Open a dumper stream.
   *
   *  Open a dumper stream.
   */
  class                          opener : public io::endpoint {
  public:
    enum                         dumper_type {
                                 dump,
                                 dump_dir,
                                 dump_fifo,
                                 db_cfg_reader,
                                 db_cfg_writer
    };

                                 opener();
                                 opener(opener const& o);
                                 ~opener();
    opener&                      operator=(opener const& o);
    misc::shared_ptr<io::stream> open();
    void                         set_db(database_config const& db_cfg);
    void                         set_path(std::string const& path);
    void                         set_tagname(std::string const& tagname);
    void                         set_type(dumper_type type);
    void                         set_cache(
                                   misc::shared_ptr<persistent_cache> cache);
    void                         set_name(std::string const& name);

   private:
    database_config              _db;
    std::string                  _name;
    std::string                  _path;
    std::string                  _tagname;
    dumper_type                  _type;
    misc::shared_ptr<persistent_cache>
                                 _cache;
  };
}

CCB_END()

#endif // !CCB_DUMPER_OPENER_HH
