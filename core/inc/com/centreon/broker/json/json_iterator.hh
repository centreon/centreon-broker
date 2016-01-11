/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_JSON_JSON_ITERATOR_HH
#  define CCB_JSON_JSON_ITERATOR_HH

#  include <vector>
#  include <string>
#  include "com/centreon/broker/json/jsmn.h"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace json {
  /**
   *  @class json_iterator json_iterator.hh "com/centreon/json/json_iterator.hh"
   *  @brief Iterate over the tokens of a json document.
   *
   *  As an example, the document:
   *  '{ "name" : "Jack", "age" : 27 }'
   *
   *  contains 5 tokens: the toplevel object, with 2 children:
   *  name, and age, each having a child each. Pair association is done through
   *  a children association: i.e the child of a key is its direct value.
   */
  class                    json_iterator {
  public:
                           json_iterator(
                             const char* js,
                             const jsmntok_t* tokens,
                             size_t token_number);
                           json_iterator(json_iterator const&);
    json_iterator&         operator=(json_iterator const&);
                           ~json_iterator();
    json_iterator&         operator++();

    enum                   type {
                           object,
                           array,
                           string,
                           number,
                           boolean,
                           null
    };
    type                   get_type() const throw();
    std::string            get_string_type() const;
    std::string            get_string() const;
    int                    get_integer() const;
    bool                   get_bool() const;
    bool                   is_null() const throw();    

    int                    children() const throw();
    json_iterator          enter_children() const throw();
    json_iterator          find_child(std::string const& name) const;

    bool                   end() const throw();

  private:
    const char*            _js;
    const jsmntok_t*        _tokens;
    size_t                 _token_number;
    size_t                 _index;

                           json_iterator();
  };
} // namespace json

CCB_END()

#endif // !CCB_JSON_JSON_ITERATOR_HH
