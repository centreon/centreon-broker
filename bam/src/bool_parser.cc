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

#include <algorithm>
#include <cctype>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/bam/bool_and.hh"
#include "com/centreon/broker/bam/bool_constant.hh"
#include "com/centreon/broker/bam/bool_equal.hh"
#include "com/centreon/broker/bam/bool_less_than.hh"
#include "com/centreon/broker/bam/bool_metric.hh"
#include "com/centreon/broker/bam/bool_more_than.hh"
#include "com/centreon/broker/bam/bool_not.hh"
#include "com/centreon/broker/bam/bool_operation.hh"
#include "com/centreon/broker/bam/bool_or.hh"
#include "com/centreon/broker/bam/bool_parser.hh"
#include "com/centreon/broker/bam/bool_service.hh"
#include "com/centreon/broker/bam/bool_xor.hh"
#include "com/centreon/broker/bam/hst_svc_mapping.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] exp_test  Boolean expression as text.
 *  @param[in] mapping   Host/service mapping (names to IDs).
 */
bool_parser::bool_parser(
               std::string const& exp_text,
               hst_svc_mapping const& mapping)
  : _mapping(mapping), _toknizr(exp_text) {
  try {
    _root = _make_boolean_exp();
  }
  catch (exceptions::msg const& e) {
    throw (exceptions::msg()
           << "BAM: error occurred while parsing boolean expression '"
           << exp_text << "': " << e.what());
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
bool_parser::bool_parser(bool_parser const& other)
  : _mapping(other._mapping),
    _root(other._root),
    _services(other._services),
    _toknizr(other._toknizr) {}

/**
 *  Destructor.
 */
bool_parser::~bool_parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
bool_parser& bool_parser::operator=(bool_parser const& other) {
  if (this != &other) {
    _root = other._root;
    _services = other._services;
  }
  return (*this);
}

/**
 *  Get services.
 *
 *  @return Boolean values that acts on a service state.
 */
bool_parser::list_service const& bool_parser::get_services() const {
  return (_services);
}

/**
 *  Get calls.
 *
 *  @return Boolean values that acts on an expression calls.
 */
bool_parser::list_call const& bool_parser::get_calls() const {
  return (_calls);
}

/**
 *  Get metrics.
 *
 *  @return  The metrics.
 */
bool_parser::list_metric const& bool_parser::get_metrics() const {
  return (_metrics);
}

/**
 *  Get expression tree.
 *
 *  @return Expression tree build with bool_value subclasses.
 */
bool_value::ptr bool_parser::get_tree() const {
  return (_root);
}

/**
 *  @brief Eat boolean expression.
 *
 *  Consumes all terms at a certain level of scope and returns a
 *  bool_value.
 *
 *  @return A bool_value that abstracts all the boolean exp.
 */
bool_value::ptr bool_parser::_make_boolean_exp() {
  bool_value::ptr result(_make_term());
  bool_binary_operator::ptr op(_make_op());
  // We read until we have consumed all terms AT a given level.
  while (!op.isNull()) {
    bool_value::ptr right_term(_make_term());
    op->set_left(result);
    op->set_right(right_term);
    result->add_parent(op.staticCast<bam::computable>());
    right_term->add_parent(op.staticCast<bam::computable>());
    result = op.staticCast<bool_value>();
    op = _make_op();
  }
  return (result);
}

/**
 *  @brief Make operator.
 *
 *  This method consumes an operator token.
 *
 *  @return  A binary operator object.
 */
bool_binary_operator::ptr bool_parser::_make_op() {
  // Note: it is perfectly legal to return an empty operator
  //       this denotes the end of an embedded boolean expression.
  bool_binary_operator::ptr result;

  if (_toknizr.char_is('{')) {
    std::string sym(_toknizr.get_token());
    std::transform(sym.begin(), sym.end(), sym.begin(), toupper);

    if (!sym.empty()) {
      if (sym == "AND")
        result = new bool_and();
      else if (sym == "OR")
        result = new bool_or();
      else if (sym == "XOR")
        result =  new bool_xor();
      else if (sym == "IS" || sym == "EQUAL" || sym == "=")
        result = new bool_equal();
      else if (sym == "<=" || sym == "<")
        result = new bool_less_than(sym == "<");
      else if (sym == ">=" || sym == ">")
        result = new bool_more_than(sym == ">");
      else if (sym == "+" || sym == "-" || sym == "*" || sym == "/")
        result = new bool_operation(sym);
      else
        throw (exceptions::msg() << "illegal operator '" << sym << "'");
    }
  }
  return (result);
}

bool_parser::term_parse_function bool_parser::_term_parse_functions[] =
    {&bool_parser::_make_constant,
     &bool_parser::_make_status,
     &bool_parser::_make_metric,
     &bool_parser::_make_aggregate,
     &bool_parser::_make_call,
     &bool_parser::_make_service};

/**
 *  @brief Eat term.
 *
 *  This method consumes a term expression.
 *
 *  @return  A bool value object representing the consumed token.
 */
bool_value::ptr bool_parser::_make_term() {
  bool_value::ptr result;

  if (_toknizr.char_is('{')) {
    if (_toknizr.token_is("NOT")) {
      // NOT operator.
      _toknizr.drop_token();
      result = new bool_not(_make_term());
    }
    else {
      // Try to parse a term.
      for (size_t i = 0;
           i < sizeof(_term_parse_functions) / sizeof(*_term_parse_functions)
           && result.isNull();
           ++i)
        (this->*_term_parse_functions[i])(_toknizr.get_token());

      if (result.isNull())
        throw (exceptions::msg()
               << "couldn't parse operand at token '"
               << _toknizr.get_token() << "'");
      _toknizr.drop_token();
    }
  }
  else if (_toknizr.char_is('(')) {
    _toknizr.drop_char();
    result = _make_boolean_exp();
    if (!_toknizr.char_is(')'))
      throw (exceptions::msg() << "missing terminating ')'");
    _toknizr.drop_char();
  }
  else
    throw (exceptions::msg() << "illegal token '"
           << _toknizr.get_token() << "'");
  _toknizr.drop_ws();
  return (result);
}

/**
 *  Parse and return a constant object.
 *
 *  @param[in] The string.
 *
 *  @return  The constant object or null.
 */
bool_value::ptr bool_parser::_make_constant(std::string const& constant) {
  int val = 0;
  bool_value::ptr retval;

  if (misc::string::to(constant.c_str(), val))
    retval = bool_value::ptr(new bool_constant(val));

  return (retval);
}

/**
 *  Parse a service from a string.
 *
 *  @param[in] str          The string to parse.
 *  @param[out] host_id     The parsed host id.
 *  @param[out] service_id  The parsed service id.
 *
 *  @return  True if parsing was succesfull.
 */
bool bool_parser::_parse_service(
                   std::string const& str,
                   unsigned int& host_id,
                   unsigned int& service_id) const {
  bool retval = false;

  if (str.find(':') == std::string::npos
      && std::count(str.begin(), str.end(), ' ') < 1) {
    size_t split(str.find(' '));
    std::string hst(str.substr(0, split));
    std::string svc;
    if (split != std::string::npos)
      svc = str.substr(split + 1);
    std::pair<unsigned int, unsigned int>
      ids(_mapping.get_service_id(hst, svc));
    if (!ids.first || !ids.second)
      throw (exceptions::msg()
             << "could not find ID of service '" << svc
             << "' or of host '" << hst << "'");
    retval = true;
    host_id = ids.first;
    service_id = ids.second;
  }
  return (retval);
}

/**
 *  Parse and return a service object.
 *
 *  @param[in] str  The string.
 *
 *  @return  The service object or null.
 */
bool_value::ptr bool_parser::_make_service(std::string const& str) {
  bool_value::ptr retval;
  unsigned int service_id;
  unsigned int host_id;

  if (_parse_service(str, host_id, service_id)) {
    bool_service::ptr ret(new bool_service);
    retval = ret;
    ret->set_host_id(host_id);
    ret->set_service_id(service_id);
    _services.push_back(ret);
  }

  return (retval);
}

/**
 *  Parse and return a status object.
 *
 *  @param[in] str  The string.
 *
 *  @return  The status object or null.
 */
bool_value::ptr bool_parser::_make_status(std::string const& str) {
  bool_value::ptr retval;

  if (str == "OK")
    retval = bool_value::ptr(new bool_constant(0));
  else if (str == "WARNING")
    retval = bool_value::ptr(new bool_constant(1));
  else if (str == "CRITICAL")
    retval = bool_value::ptr(new bool_constant(2));
  else if (str == "UNKNOWN")
    retval = bool_value::ptr(new bool_constant(3));
  else if (str == "PENDING")
    retval = bool_value::ptr(new bool_constant(4));

  return (retval);
}

/**
 *  Parse a metric from a string.
 *
 *  @param[in] str            The string.
 *  @param[out] metric_name   The parsed name of the metric.
 *  @param[in] host_id        The parsed id of the host.
 *  @param[in] service_id     The parsed id of the service.
 *
 *  @return                   True if parsed succesfully.
 */
bool bool_parser::_parse_metric(
                    std::string const& str,
                    std::string& metric_name,
                    unsigned int& host_id,
                    unsigned int& service_id) const {
  if (str.compare(0, ::strlen("metric:"), "metric:") != 0)
    return (false);

  size_t end = str.find(' ', ::strlen("metric:") + 1);
  if (end == std::string::npos)
    end = str.size();
  metric_name = str.substr(::strlen("metric:"), end);
  misc::string::trim(metric_name);

  std::string node = metric_name.substr(end);
  misc::string::trim(node);
  if (!node.empty()) {
    if (node[0] != '{' || node.size() <= 2 || node[1] != '}')
      throw (exceptions::msg()
             << "couldn't parse metric '" << str << "'");
    node.erase(0, 1);
    node.erase(node.size() - 1, 1);
    if (!_parse_service(node, host_id, service_id))
      throw (exceptions::msg()
             << "couldn't parse metric node in '" << str << "'");
  }

  return (true);
}

/**
 *  Parse and return a metric object.
 *
 *  @param[in] str  The string.
 *
 *  @return  The metric object or null.
 */
bool_value::ptr bool_parser::_make_metric(std::string const& str) {
  bool_value::ptr retval;
  std::string metric_name;
  unsigned int host_id;
  unsigned int service_id;

  if (_parse_metric(str, metric_name, host_id, service_id)) {
    bool_metric::ptr metric(new bool_metric(metric_name, host_id, service_id));
    retval = metric;
    _metrics.push_back(metric);
  }

  return (retval);
}

/**
 *  Parse and return an aggregate object.
 *
 *  @param[in] str  The string.
 *
 *  @return  The aggregate object or null.
 */
bool_value::ptr bool_parser::_make_aggregate(std::string const& str) {
  bool_value::ptr retval;

  return (retval);
}

/**
 *  Parse and return a call object.
 *
 *   @param[in] str  The string.
 *
 *  @return  The call object or null.
 */
bool_value::ptr bool_parser::_make_call(std::string const& str) {
  bool_value::ptr retval;

  if (str.compare(0, ::strlen("call:"), "call:") == 0) {
    std::string name = str.substr(::strlen("call:") + 1);
    misc::string::trim(name);
    bool_call::ptr call(new bool_call(name));
    retval = call;
    _calls.push_back(call);
  }

  return (retval);
}
