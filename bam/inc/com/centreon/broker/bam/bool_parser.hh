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
#  include "com/centreon/broker/bam/bool_metric.hh"
#  include "com/centreon/broker/bam/bool_aggregate.hh"
#  include "com/centreon/broker/bam/bool_tokenizer.hh"
#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/bam/bool_call.hh"
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
   *
   *  begin <- expression
   *  expression <- (term operator)* term
   *  term <- constant / node / metric / aggregation / status / \(expression\) / call / {NOT} term
   *  operator <- {IS} / {NOT} / {OR} / {AND} / {>} / {<} / {>=} / {<=} / {EQUAL} / {+} / {-} / {*} / {/} / {%}
   *  constant <- {(-/+)?[0-9]+}
   *  node <- {literal literal?}
   *  metric <- {metric: literal node?}
   *  aggregation <- {aggregation_type: (metric,)* metric}
   *  aggregation_type <- AVERAGE / SUM / MIN / MAX / COUNT
   *  call <- {call: literal}
   *  status <- {OK} / {CRITICAL} / {WARNING} / {UNKNOWN}
   *  literal <- [:word:]
   */
  class              bool_parser{
  public:
    typedef std::list<bool_service::ptr> list_service;
    typedef std::list<bool_metric::ptr> list_metric;
    typedef std::list<bool_call::ptr> list_call;
    typedef std::list<bool_aggregate::ptr> list_aggregate;

                     bool_parser(
                       std::string const& exp_text,
                       hst_svc_mapping const& mapping);
                     bool_parser(bool_parser const& other);
                     ~bool_parser();
    bool_parser&     operator=(bool_parser const& other);
    list_service const&
                     get_services() const;
    list_call const&
                     get_calls() const;
    list_metric const&
                     get_metrics() const;
    list_aggregate const&
                     get_aggregates() const;
    bool_value::ptr  get_tree() const;

  private:
    hst_svc_mapping const&
                     _mapping;
    bool_value::ptr  _root;
    list_service     _services;
    list_call        _calls;
    list_metric      _metrics;
    list_aggregate   _aggregates;
    bool_tokenizer   _toknizr;

    bool_binary_operator::ptr
                     _make_op();
    bool_value::ptr  _make_term();

    // Terms
    typedef bool_value::ptr
                      (bool_parser::*term_parse_function)(std::string const&);

    static term_parse_function
                      _term_parse_functions[];
    bool_value::ptr   _make_constant(std::string const& constant);
    bool              _parse_service(
                        std::string const& str,
                        unsigned int& host_id,
                        unsigned int& service_id) const;
    bool_value::ptr   _make_service(std::string const& str);
    bool_value::ptr   _make_status(std::string const& str);
    bool              _parse_metric(
                        std::string const& str,
                        std::string& metric_name,
                        unsigned int& host_id,
                        unsigned int& service_id) const;
    bool_value::ptr   _make_metric(std::string const& str);
    bool_value::ptr   _make_aggregate(std::string const& str);
    bool_value::ptr   _make_call(std::string const& str);

    bool_value::ptr  _make_boolean_exp();
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_PARSER_HH
