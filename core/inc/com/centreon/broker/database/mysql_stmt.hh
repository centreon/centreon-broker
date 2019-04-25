/*
** Copyright 2018 Centreon
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

#ifndef CCB_MYSQL_STMT_HH
#  define CCB_MYSQL_STMT_HH

#  include <map>
#  include <memory>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/database/mysql_bind.hh"

CCB_BEGIN()

typedef                         std::map<std::string, int> mysql_bind_mapping;

namespace                       database {
  class                           mysql_stmt {
   public:
                                  mysql_stmt();
                                  mysql_stmt(
                                    std::string const& query,
                                    bool named);
                                  mysql_stmt(
                                    std::string const& query,
                                    mysql_bind_mapping const& bind_mapping = mysql_bind_mapping());
                                  mysql_stmt(mysql_stmt&& other);
    mysql_stmt&                   operator=(mysql_stmt const& other);
    bool                          prepared() const;
    int                           get_id() const;
    std::unique_ptr<database::mysql_bind>
                                  get_bind();
    void                          operator<<(io::data const& d);

    void                          bind_value_as_i32(int range, int value);
    void                          bind_value_as_i32(std::string const& key, int value);

    void                          bind_value_as_u32(int range, unsigned int value);
    void                          bind_value_as_u32(std::string const& key, unsigned int value);

    void                          bind_value_as_u64(int range, unsigned long long value);
    void                          bind_value_as_u64(std::string const& key, unsigned long long value);

    void                          bind_value_as_f32(int range, float value);
    void                          bind_value_as_f32(std::string const& key, float value);

    void                          bind_value_as_f64(int range, double value);
    void                          bind_value_as_f64(std::string const& key, double value);

    void                          bind_value_as_tiny(int range, char value);
    void                          bind_value_as_tiny(std::string const& key, char value);

    void                          bind_value_as_bool(int range, bool value);
    void                          bind_value_as_bool(std::string const& key, bool value);

    void                          bind_value_as_str(int range, std::string const& value);
    void                          bind_value_as_str(std::string const& key, std::string const& value);

    void                          bind_value_as_null(int range);
    void                          bind_value_as_null(std::string const& key);
    std::string const&            get_query() const;
    int                           get_param_count() const;

   private:
    int                           _compute_param_count(std::string const& query);

    int                           _id;
    int                           _param_count;
    std::string                   _query;

    std::unique_ptr<database::mysql_bind>
                                  _bind;
    mysql_bind_mapping            _bind_mapping;
  };
}

CCB_END()

#endif // CCB_MYSQL_STMT_HH
