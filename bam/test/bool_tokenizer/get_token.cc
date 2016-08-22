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
#include "com/centreon/broker/bam/bool_tokenizer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

// Given a bool_tokenizer object
// When it is constructed with a valid expression
// Then get_token() returns the tokens one after the other
TEST(BamBoolTokenizerGetToken, Valid) {
  bam::bool_tokenizer bt("{Centreon-Server Load} {IS} {OK}");
  ASSERT_EQ(bt.get_token(), "Centreon-Server Load");
  ASSERT_EQ(bt.get_token(), "IS");
  ASSERT_EQ(bt.get_token(), "OK");
}

// Given a bool_tokenizer object
// When it is constructed with a valid expression
// Then get_token() throws when there are no more tokens
TEST(BamBoolTokenizerGetToken, ThrowAtEnd) {
  bam::bool_tokenizer bt("{Centreon-Server Load} {IS} {OK}");
  for (int i(0); i < 3; ++i)
    bt.get_token();
  ASSERT_THROW(bt.get_token(), exceptions::msg);
}
