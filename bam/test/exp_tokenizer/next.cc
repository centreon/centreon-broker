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
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "Host");
  ASSERT_EQ(toknzr.next(), "Service");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "IS");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "OK");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Valid2) {
  bam::exp_tokenizer
    toknzr("{AVERAGE: {metric: trafficin}, {metric: traffic_in {Host \\Service}}} {+} {42}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "AVERAGE");
  ASSERT_EQ(toknzr.next(), ":");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "metric");
  ASSERT_EQ(toknzr.next(), ":");
  ASSERT_EQ(toknzr.next(), "trafficin");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), ",");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "metric");
  ASSERT_EQ(toknzr.next(), ":");
  ASSERT_EQ(toknzr.next(), "traffic_in");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "Host");
  ASSERT_EQ(toknzr.next(), "Service");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "+");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "42");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with a valid expression
// Then next() returns the tokens one after the other
TEST(BamExpTokenizerNext, Valid3) {
  bam::exp_tokenizer toknzr("{metric:\"my 'out\' \\\"METR:C\\\"\" {'{Host}' '{Serv\\ice}'}}");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "metric");
  ASSERT_EQ(toknzr.next(), ":");
  ASSERT_EQ(toknzr.next(), "my 'out' \"METR:C\"");
  ASSERT_EQ(toknzr.next(), "{");
  ASSERT_EQ(toknzr.next(), "{Host}");
  ASSERT_EQ(toknzr.next(), "{Serv\\ice}");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "}");
  ASSERT_EQ(toknzr.next(), "");
}

// Given an exp_tokenizer object
// When it is constructed with an invalid expression
// Then next() will throw when the invalid syntax is parsed
TEST(BamExpTokenizerNext, UnterminatedSingleQuote) {
  bam::exp_tokenizer toknzr("{'MyHost' 'MyService} {IS} {'OK'}");
  ASSERT_THROW({
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
  ASSERT_THROW({
      for (int i(0); i < 100; ++i)
        toknzr.next();
    },
    exceptions::msg);
}
