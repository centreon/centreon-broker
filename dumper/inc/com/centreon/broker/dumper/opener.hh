/*
** Copyright 2013 Centreon
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
