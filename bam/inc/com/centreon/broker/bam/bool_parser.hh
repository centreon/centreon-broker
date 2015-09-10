/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_BOOL_PARSER_HH
#  define CCB_BAM_BOOL_PARSER_HH

#  include <list>
#  include <string>
#  include "com/centreon/broker/bam/bool_binary_operator.hh"
#  include "com/centreon/broker/bam/bool_service.hh"
#  include "com/centreon/broker/bam/bool_tokenizer.hh"
#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            bam {
  // Forward declaration.
  class              hst_svc_mapping;

  /**
   *  @class bool_parser bool_parser.hh "com/centreon/broker/bam/bool_parser.hh"
   *  @brief Boolean expression parser.
   *
   *  A boolean expression parser for interpreting BAM expressions
   *  consisting of a boolean-type syntax.
   */
  class              bool_parser{
  public:
    typedef std::list<bool_service::ptr> list_service;

                     bool_parser(
                       std::string const& exp_text,
                       hst_svc_mapping const& mapping);
                     bool_parser(bool_parser const& other);
                     ~bool_parser();
    bool_parser&     operator=(bool_parser const& other);
    list_service const&
                     get_services() const;
    bool_value::ptr  get_tree() const;

  private:
    hst_svc_mapping const&
                     _mapping;
    bool_value::ptr  _root;
    list_service     _services;
    bool_tokenizer   _toknizr;

    bool_value::ptr  _make_boolean_exp();
    bool_value::ptr  _make_host_service_state();
    bool_binary_operator::ptr
                     _make_op();
    bool_value::ptr  _make_term();
    bool             _token_to_condition(std::string const& token);
    short            _token_to_service_state(std::string const& token);
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_PARSER_HH
