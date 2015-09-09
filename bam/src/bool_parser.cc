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
#include "com/centreon/broker/bam/bool_and.hh"
#include "com/centreon/broker/bam/bool_not.hh"
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
 *  @brief Make host service state.
 *
 *  Turns a token into a boolean node.
 *
 *  @return Return bool value
 */
bool_value::ptr bool_parser::_make_host_service_state() {
  // FORMAT example   {HOST SERV} {is} {UNKNOWN}

  // Get host.
  std::string hst_svc(_toknizr.get_token());
  size_t split(hst_svc.find(' '));
  if (split == std::string::npos)
    throw (exceptions::msg()
           << "service must be expressed as {HOST SERVICE}");
  std::string hst(hst_svc.substr(0, split));
  std::string svc(hst_svc.substr(split + 1));
  std::pair<unsigned int, unsigned int>
    ids(_mapping.get_service_id(hst, svc));
  if (!ids.first || !ids.second)
    throw (exceptions::msg()
           << "could not find ID of service '" << svc
           << "' or of host '" << hst << "'");

  // Condition whether state desired OR not
  bool is_expected(_token_to_condition(_toknizr.get_token()));

  // Get referred state
  short state(_token_to_service_state(_toknizr.get_token()));

  // Create object.
  bool_service::ptr ret(new bool_service);
  ret->set_host_id(ids.first);
  ret->set_service_id(ids.second);
  ret->set_value_if_state_match(is_expected);
  ret->set_expected_state(state);
  _services.push_back(ret);
  return (ret.staticCast<bool_value>());
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
      else
        throw (exceptions::msg() << "illegal operator '" << sym << "'");
    }
  }
  return (result);
}

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
      // Host_service construct.
      result = _make_host_service_state();
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
 *  Convert a plain condition (IS or NOT) to a boolean.
 *
 *  @param[in] token Either "IS" or "NOT".
 *
 *  @return True if IS, false if NOT.
 */
bool bool_parser::_token_to_condition(std::string const& token) {
  std::string be(token);
  std::transform(be.begin(), be.end(), be.begin(), toupper);
  bool retval;
  if (be == "IS")
    retval = true;
  else if (be == "NOT")
    retval = false;
  else
    throw (exceptions::msg() << "invalid condition '" << be << "'");
  return (retval);
}

/**
 *  Convert a plain STATE to its values.
 *
 *  @param[in] token WARNING, CRITICAL, UNKNOWN, PENDING or OK.
 *
 *  @return The numeric state matching token.
 */
short bool_parser::_token_to_service_state(std::string const& token) {
  std::string serv_state(token);
  std::transform(
         serv_state.begin(),
         serv_state.end(),
         serv_state.begin(),
         toupper);
  short retval;
  if (serv_state == "OK")
    retval = 0;
  else if (serv_state == "WARNING")
    retval = 1;
  else if (serv_state == "CRITICAL")
    retval = 2;
  else if (serv_state == "UNKNOWN")
    retval = 3;
  else if (serv_state == "PENDING")
    retval = 4;
  else
    throw (exceptions::msg() << "invalid state '" << serv_state << "'");
  return (retval);
}
