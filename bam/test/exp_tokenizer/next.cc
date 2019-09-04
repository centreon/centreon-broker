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
#include "com/centreon/broker/bam/exp_tokenizer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Valid1) {
  bam::exp_tokenizer toknzr("{Host Service} {IS} {OK}");
  ASSERT_EQ(toknzr.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Service");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "OK");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Valid2) {
  bam::exp_tokenizer toknzr(
      "AVERAGE(METRICS(trafficin), METRIC(traffic_in, "
      "Host, \\Service) + 42)");
  ASSERT_EQ(toknzr.next(), "AVERAGE");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "METRICS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "trafficin");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "METRIC");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "traffic_in");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Host");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Service");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "+");
  ASSERT_EQ(toknzr.next(), "42");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Valid3) {
  bam::exp_tokenizer toknzr(
      "METRIC(\"my 'out\' \\\"METR:C\\\"\", 'Host', "
      "'Serv\\ice')");
  ASSERT_EQ(toknzr.next(), "METRIC");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "my 'out' \"METR:C\"");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Host");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Serv\\ice");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, NoSpaces1) {
  bam::exp_tokenizer toknzr(
      "!HOSTSTATUS('MyHost1')!=HOSTSTATUS('MyHost2')||"
      "HOSTSTATUS(\"MyHost3\")");
  ASSERT_EQ(toknzr.next(), "!");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "MyHost1");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "!=");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "MyHost2");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "||");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "MyHost3");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, NoSpaces2) {
  bam::exp_tokenizer toknzr(
      "SERVICESTATUS('MyHost1','MyService1')!=OK||"
      "(42+36==SERVICESTATUS('MyHost2',\"MyService2\"))");
  ASSERT_EQ(toknzr.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "MyHost1");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "MyService1");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "!=");
  ASSERT_EQ(toknzr.next(), "OK");
  ASSERT_EQ(toknzr.next(), "||");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "42");
  ASSERT_EQ(toknzr.next(), "+");
  ASSERT_EQ(toknzr.next(), "36");
  ASSERT_EQ(toknzr.next(), "==");
  ASSERT_EQ(toknzr.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "MyHost2");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "MyService2");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression of the old syntax
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, OldSyntax1) {
  bam::exp_tokenizer toknzr(
      "{Host1 Service1} {IS} {OK} AND ({Host2} {NOT} "
      "{DOWN} OR {Host3} {IS} {UNREACHABLE})");
  ASSERT_EQ(toknzr.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host1");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Service1");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "OK");
  ASSERT_EQ(toknzr.next(), "AND");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host2");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "NOT");
  ASSERT_EQ(toknzr.next(), "DOWN");
  ASSERT_EQ(toknzr.next(), "OR");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host3");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "UNREACHABLE");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

TEST(BamExpTokenizerNext, OldSyntax2) {
  bam::exp_tokenizer toknzr(
      "{Host1 Service1} {IS} {OK} AND ({Host2} {NOT} "
      "{DOWN} XOR {Host3} {IS} {UNREACHABLE})");
  ASSERT_EQ(toknzr.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host1");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "Service1");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "OK");
  ASSERT_EQ(toknzr.next(), "AND");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host2");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "NOT");
  ASSERT_EQ(toknzr.next(), "DOWN");
  ASSERT_EQ(toknzr.next(), "XOR");
  ASSERT_EQ(toknzr.next(), "HOSTSTATUS");
  ASSERT_EQ(toknzr.next(), "(");
  ASSERT_EQ(toknzr.next(), "Host3");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "UNREACHABLE");
  ASSERT_EQ(toknzr.next(), ")");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Copy) {
  bam::exp_tokenizer toknzr("{Host Service} {IS} {OK}");
  bam::exp_tokenizer toknzr_copy(toknzr);
  bam::exp_tokenizer toknzr_assign("");

  toknzr_assign = toknzr_copy;
  ASSERT_EQ(toknzr_assign.next(), "SERVICESTATUS");
  ASSERT_EQ(toknzr_assign.next(), "(");
  ASSERT_EQ(toknzr_assign.next(), "Host");
  ASSERT_EQ(toknzr_assign.next(), ",");
  ASSERT_EQ(toknzr_assign.next(), "Service");
  ASSERT_EQ(toknzr_assign.next(), ")");
  ASSERT_EQ(toknzr_assign.next(), "IS");
  ASSERT_EQ(toknzr_assign.next(), "OK");
  ASSERT_EQ(toknzr_assign.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with an invalid expression
// Then next() will throw when the invalid syntax is parsed
TEST(BamExpTokenizerNext, UnterminatedSingleQuote) {
  bam::exp_tokenizer toknzr("{'MyHost' 'MyService} {IS} {'OK'}");
  ASSERT_THROW(
      {
        for (int i(0); i < 100; ++i)
          toknzr.next();
      },
      exceptions::msg);
}

// Given an exp_tokenizer object
// When it is constructed with an invalid expression
// Then next() will throw when the invalid syntax is parsed
TEST(BamExpTokenizerNext, UnterminatedDoubleQuote) {
  bam::exp_tokenizer toknzr("{\"MyHost\" \"MyService} {IS} {\"OK\"}");
  ASSERT_THROW(
      {
        for (int i(0); i < 100; ++i)
          toknzr.next();
      },
      exceptions::msg);
}

// Given an exp_tokenizer object
// When it is constructed with an invalid expression
// Then next() will throw when the invalid syntax is parsed
TEST(BamExpTokenizerNext, UnterminatedBrace1) {
  bam::exp_tokenizer toknzr("{MyHost Service} {IS} {OK");
  ASSERT_THROW(
      {
        for (int i(0); i < 100; ++i)
          toknzr.next();
      },
      exceptions::msg);
}

// Given an exp_tokenizer object
// When it is constructed with an invalid expression
// Then next() will throw when the invalid syntax is parsed
TEST(BamExpTokenizerNext, UnterminatedBrace2) {
  bam::exp_tokenizer toknzr("{MyHost Service} {IS {OK}");
  ASSERT_THROW(
      {
        for (int i(0); i < 100; ++i)
          toknzr.next();
      },
      exceptions::msg);
}

// Given an exp_tokenizer object
// When it is constructed with an valid expression
// Then next() will return all tokens one after the other
TEST(BamExpTokenizerNext, UnterminatedBrace3) {
  bam::exp_tokenizer toknzr("{MyHost Service {IS} {OK}");
  ASSERT_NO_THROW({
    for (int i(0); i < 100; ++i)
      toknzr.next();
  });
}
