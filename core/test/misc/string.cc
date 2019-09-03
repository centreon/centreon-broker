/*
** Copyright 2012-2013,2015 Centreon
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

#include "com/centreon/broker/misc/string.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker::misc;

TEST(StringSplit, OnePart) {
  std::list<std::string> lst{string::split("test", ' ')};
  ASSERT_EQ(lst.size(), 1u);
  ASSERT_EQ(lst.front(), "test");
}

TEST(StringSplit, ThreePart) {
  std::list<std::string> lst{string::split("test foo bar", ' ')};
  ASSERT_EQ(lst.size(), 3u);
  std::list<std::string> res{"test", "foo", "bar"};
  ASSERT_EQ(lst, res);
}

TEST(StringSplit, ManyPart) {
  std::list<std::string> lst{
      string::split("  test foo bar a b  c d eeeee", ' ')};
  ASSERT_EQ(lst.size(), 11u);
  std::list<std::string> res{"",  "", "test", "foo", "bar",  "a",
                             "b", "", "c",    "d",   "eeeee"};
  ASSERT_EQ(lst, res);
}

TEST(StringBase64, Encode) {
  ASSERT_EQ(string::base64_encode("A first little attempt."),
            "QSBmaXJzdCBsaXR0bGUgYXR0ZW1wdC4=");
  ASSERT_EQ(string::base64_encode("A"), "QQ==");
  ASSERT_EQ(string::base64_encode("AB"), "QUI=");
  ASSERT_EQ(string::base64_encode("ABC"), "QUJD");
}
