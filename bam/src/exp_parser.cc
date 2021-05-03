/*
** Copyright 2016 Centreon
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

#include "com/centreon/broker/bam/exp_parser.hh"

#include <cstdlib>
#include <sstream>
#include <stack>

#include "com/centreon/broker/bam/exp_tokenizer.hh"
#include "com/centreon/exceptions/msg_fmt.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] expression  Expression to parse.
 */
exp_parser::exp_parser(std::string const& expression) : _exp(expression) {
  _precedence["&&"] = 2;
  _precedence["AND"] = 2;
  _precedence["||"] = 2;
  _precedence["OR"] = 2;
  _precedence["IS"] = 3;
  _precedence["^"] = 2;
  _precedence["XOR"] = 2;
  _precedence["NOT"] = 3;
  _precedence[">"] = 3;
  _precedence[">="] = 3;
  _precedence["<"] = 3;
  _precedence["<="] = 3;
  _precedence["=="] = 3;
  _precedence["!="] = 3;
  _precedence["+"] = 4;
  _precedence["-"] = 4;
  _precedence["*"] = 5;
  _precedence["/"] = 5;
  _precedence["%"] = 5;
  _precedence["-u"] = 6;
  _precedence["!"] = 6;

  log_v2::bam()->trace("exp_parser constructor '{}'", expression);
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
exp_parser::exp_parser(exp_parser const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
exp_parser::~exp_parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
exp_parser& exp_parser::operator=(exp_parser const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Get postfix notation of expression.
 *
 *  @return Postfix notation of expression.
 */
exp_parser::notation const& exp_parser::get_postfix() {
  // Store tokens in list.
  std::list<std::string> tokens;
  {
    exp_tokenizer toknzr(_exp);
    std::string token;
    token = toknzr.next();
    while (!token.empty()) {
      tokens.push_back(token);
      token = toknzr.next();
    }
  }

  /*
  ** SHUNTING YARD ALGORITHM.
  **
  ** This was implemented by using Wikipedia's algorithm description.
  ** Refer to https://en.wikipedia.org/wiki/Shunting_yard_algorithm
  ** for more information.
  */

  // While there are tokens to be read.
  bool can_be_unary(true);
  std::stack<std::string> stack;
  std::stack<int> arity;
  while (!tokens.empty()) {
    // Read a token.
    std::string token(tokens.front());
    tokens.pop_front();

    // If token is a function, then push it onto the stack.
    if (is_function(token)) {
      stack.push(token);
      arity.push(1);
      can_be_unary = false;
    }
    // If token is a comma, ...
    else if (token == ",") {
      // Pop operator tokens off the stack to output queue until
      // left parenthesis.
      while (!stack.empty() && (stack.top() != "(")) {
        _postfix.push_back(stack.top());
        stack.pop();
      }
      if (stack.empty()) {
        throw msg_fmt(
            "mismatched parentheses found while parsing "
            "the following expression: {}",
            _exp);
      }

      // Increment function arity.
      if (arity.empty()) {
        throw msg_fmt(
            "found comma outside function call while parsing "
            "the following expression: {}",
            _exp);
      }
      ++arity.top();

      // Next token cannot be unary.
      can_be_unary = false;
    }
    // If token is an operator o1, then...
    else if (is_operator(token)) {
      // Handle unary operators.
      if (can_be_unary && (token == "-")) {
        stack.push("-u");
      } else if (can_be_unary && ((token == "NOT") || (token == "!"))) {
        stack.push("!");
      } else {
        // While there is an operator token o2 at the top of the stack...
        while (!stack.empty() &&
               is_operator(stack.top())
               // And o1's precedence is less than or equal to that of o2
               && (_precedence[token] <= (_precedence[stack.top()]))) {
          // Pop o2 off the operator stack, onto the output queue.
          _postfix.push_back(stack.top());
          stack.pop();
        }
        // At the end of iteratation push o1 onto the operator stack.
        stack.push(token);
      }
      can_be_unary = true;
    }
    // If token is a left parenthesis, then push it onto the stack.
    else if (token == "(") {
      stack.push(token);
      can_be_unary = true;
    }
    // If token is a right parenthesis...
    else if (token == ")") {
      // Pop operator tokens off the stack to output queue until
      // left parenthesis.
      while (!stack.empty() && (stack.top() != "(")) {
        _postfix.push_back(stack.top());
        stack.pop();
      }
      if (stack.empty()) {
        throw msg_fmt(
            "mismatched parentheses found while parsing "
            "the following expression: {}",
            _exp);
      }

      // Pop left parenthesis off the stack.
      stack.pop();

      // If the token at the top of the stack is a function, pop it
      // onto the output queue.
      if (!stack.empty() && is_function(stack.top())) {
        _postfix.push_back(stack.top());
        stack.pop();
        std::ostringstream oss;
        oss << arity.top();
        _postfix.push_back(oss.str());
        arity.pop();
      }

      // Next token cannot be unary.
      can_be_unary = false;
    }
    // If token is a number, then add it to the output queue.
    else {
      _postfix.push_back(token);
      can_be_unary = false;
    }
  }

  // When there are no more tokens to read.
  while (!stack.empty()) {
    // If operator is a parenthesis, then it is mismatched.
    std::string token(stack.top());
    stack.pop();
    if (token == "(") {
      throw msg_fmt(
          "mismatched parentheses found while parsing "
          "the following expression: {}",
          _exp);
    }
    // Or pop the operator onto the output queue.
    _postfix.push_back(token);
  }

  return (_postfix);
}

/**
 *  Check if token is a valid function name.
 *
 *  @return True if token is a valid function name.
 */
bool exp_parser::is_function(std::string const& token) {
  return ((token == "HOSTSTATUS") || (token == "SERVICESTATUS") ||
          (token == "METRICS") || (token == "METRIC") || (token == "AVERAGE") ||
          (token == "COUNT") || (token == "MAX") || (token == "MIN") ||
          (token == "SUM") || (token == "CALL"));
}

/**
 *  Check if token is a valid operator.
 *
 *  @return True if token is a valid operator.
 */
bool exp_parser::is_operator(std::string const& token) {
  return ((token == "+") || (token == "-") || (token == "-u") ||
          (token == "*") || (token == "/") || (token == "%") ||
          (token == ">") || (token == ">=") || (token == "<") ||
          (token == "<=") || (token == "==") || (token == "IS") ||
          (token == "NOT") || (token == "!=") || (token == "!") ||
          (token == "AND") || (token == "^") || (token == "XOR") ||
          (token == "&&") || (token == "OR") || (token == "||"));
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void exp_parser::_internal_copy(exp_parser const& other) {
  _exp = other._exp;
  _postfix = other._postfix;
  _precedence = other._precedence;
  return;
}
