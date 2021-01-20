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

#include <gtest/gtest.h>
#include <list>
#include <string>
#include "com/centreon/broker/bam/exp_parser.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

/**
 *  Helper function.
 */
static std::list<std::string> array_to_list(char const* array[]) {
  std::list<std::string> retval;
  for (int i(0); array[i]; ++i)
    retval.push_back(array[i]);
  return (retval);
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() returns its postfix notation
TEST(BamExpParserGetPostfix, Valid1) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS OK");
  char const* expected[] = {"Host", "Service", "HOSTSTATUS", "2",
                            "OK",   "IS",      nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, Valid2) {
  bam::exp_parser p(
      "AVERAGE(METRICS('ping'), METRIC(\"my_ping\", Host, "
      "Service)) >= (SUM(METRICS('limit'), METRICS('limit_offset')) + 42)");
  char const* expected[] = {
      "ping",    "METRICS", "1", "my_ping",      "Host",
      "Service", "METRIC",  "3", "AVERAGE",      "2",
      "limit",   "METRICS", "1", "limit_offset", "METRICS",
      "1",       "SUM",     "2", "42",           "+",
      ">=",      nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, Valid3) {
  bam::exp_parser p(
      "{Host1 Service1} {IS} {OK} {AND} {Host2 Service2} {IS} {OK}");
  char const* expected[] = {
      "Host1", "Service1", "SERVICESTATUS", "2", "OK", "IS",
      "Host2", "Service2", "SERVICESTATUS", "2", "OK", "IS",
      "AND",   nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, Valid4) {
  bam::exp_parser p(
      "SERVICESTATUS('MyHost1','MyService1')!=OK||(42+36==SERVICESTATUS('"
      "MyHost2',\"MyService2\"))");
  char const* expected[] = {"MyHost1", "MyService1", "SERVICESTATUS",
                            "2",       "OK",         "!=",
                            "42",      "36",         "+",
                            "MyHost2", "MyService2", "SERVICESTATUS",
                            "2",       "==",         "||",
                            nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, Valid5) {
  bam::exp_parser p(
      "{poller-paris Cpu} {NOT} {OK}\n"
      "{OR}\n"
      "{poller-paris Disk-/} {NOT} {OK}\n"
      "{OR}\n"
      "{poller-paris Load} {NOT} {OK}\n");
  char const* expected[] = {"poller-paris",
                            "Cpu",
                            "SERVICESTATUS",
                            "2",
                            "OK",
                            "NOT",
                            "poller-paris",
                            "Disk-/",
                            "SERVICESTATUS",
                            "2",
                            "OK",
                            "NOT",
                            "OR",
                            "poller-paris",
                            "Load",
                            "SERVICESTATUS",
                            "2",
                            "OK",
                            "NOT",
                            "OR",
                            nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, Valid6) {
  bam::exp_parser p(
      "{Host1 Service1} {IS} {OK} {XOR} {Host2 Service2} {IS} "
      "{OK}");
  char const* expected[] = {
      "Host1", "Service1", "SERVICESTATUS", "2", "OK", "IS",
      "Host2", "Service2", "SERVICESTATUS", "2", "OK", "IS",
      "XOR",   nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, UnaryNot) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS NOT OK");
  char const* expected[] = {"Host", "Service", "HOSTSTATUS", "2",
                            "OK",   "!",       "IS",         nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
TEST(BamExpParserGetPostfix, UnaryMinus) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS - OK");
  char const* expected[] = {"Host", "Service", "HOSTSTATUS", "2",
                            "OK",   "-u",      "IS",         nullptr};
  ASSERT_EQ(p.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When it is constructed with a valid expression
// Then get_postfix() return its postfix notation
// Here we test copy constructor and operator=
TEST(BamExpParserGetPostfix, Copy) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS OK");
  bam::exp_parser p_copy(p);
  bam::exp_parser p_assign("");
  p_assign = p_copy;
  char const* expected[] = {"Host", "Service", "HOSTSTATUS", "2",
                            "OK",   "IS",      nullptr};
  ASSERT_EQ(p_copy.get_postfix(), array_to_list(expected));
  ASSERT_EQ(p_assign.get_postfix(), array_to_list(expected));
}

// Given an exp_parser object
// When a parenthesis is missing
// Then get_postfix() throw an exception
TEST(BamExpParserGetPostfix, MissingParenthesis1) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS OK(");
  ASSERT_THROW(p.get_postfix(), msg_fmt);
}

// Given an exp_parser object
// When a parenthesis is missing
// Then get_postfix() throw an exception
TEST(BamExpParserGetPostfix, MissingParenthesis2) {
  bam::exp_parser p("HOSTSTATUS(Host, Service)) IS OK");
  ASSERT_THROW(p.get_postfix(), msg_fmt);
}

// Given an exp_parser object
// When a comma is not right
// Then get_postfix() throw an exception
TEST(BamExpParserGetPostfix, BadComma1) {
  bam::exp_parser p("HOSTSTATUS(Host, Service), IS OK");
  ASSERT_THROW(p.get_postfix(), msg_fmt);
}

// Given an exp_parser object
// When a comma is not right
// Then get_postfix() throw an exception
TEST(BamExpParserGetPostfix, BadComma2) {
  bam::exp_parser p("HOSTSTATUS(Host, Service) IS OK,");
  ASSERT_THROW(p.get_postfix(), msg_fmt);
}

// Given an exp_parser object
// When a comma is not right
// Then get_postfix() throw an exception
TEST(BamExpParserGetPostfix, BadComma3) {
  bam::exp_parser p("(OK,OK)");
  ASSERT_THROW(p.get_postfix(), msg_fmt);
}
