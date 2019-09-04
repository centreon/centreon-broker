/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/misc/stringifier.hh"
#include <gtest/gtest.h>
#include <math.h>

using namespace com::centreon::broker;

TEST(Stringifier, Assignment) {
  // First object.
  misc::stringifier s1;
  s1 << 42 << 36987410258ull << "foobar baz qux";

  // Second object.
  misc::stringifier s2;
  s2 << "baz" << -74651211 << false;

  // Assign first object to second.
  s2 = s1;

  // Change first object.
  s1.reset();
  s1 << "qux bazbar foo" << 65478847451578ull << -3612;

  // Check data.
  ASSERT_STREQ(s1.data(), "qux bazbar foo65478847451578-3612");
  ASSERT_STREQ(s2.data(), "4236987410258foobar baz qux");
}

TEST(Stringifier, bool) {
  // First insertion.
  misc::stringifier s;
  s << false;
  ASSERT_STREQ(s.data(), "false");

  // Second insertion.
  s << true;
  ASSERT_STREQ(s.data(), "falsetrue");

  // Third insertions.
  s << false << true << false;
  ASSERT_STREQ(s.data(), "falsetruefalsetruefalse");
}

TEST(Stringifier, CopyCtor) {
  // First object.
  misc::stringifier s1;
  s1 << 42 << 36987410258ull << "foobar baz qux";

  // Second object.
  misc::stringifier s2(s1);

  // Change first object.
  s1.reset();
  s1 << "qux bazbar foo" << 65478847451578ull << -3612;

  // Check data.
  ASSERT_STREQ(s1.data(), "qux bazbar foo65478847451578-3612");
  ASSERT_STREQ(s2.data(), "4236987410258foobar baz qux");
}

TEST(Stringifier, DefaultCtor) {
  misc::stringifier s;
  ASSERT_EQ(s.data()[0], '\0');
}

TEST(Stringifier, double) {
  // First insertion.
  misc::stringifier s1;
  s1 << -36.0;
  ASSERT_FALSE(fabs(strtod(s1.data(), NULL) + 36.0) > 0.1);

  // Second insertions.
  misc::stringifier s2;
  s2 << 75697.248;
  ASSERT_FALSE(fabs(strtod(s2.data(), NULL) - 75697.248) > 0.1);
}

TEST(Stringifier, int) {
  // First insertion.
  misc::stringifier s;
  s << 42;
  ASSERT_STREQ(s.data(), "42");

  // Second insertions.
  s << -7895456 << 236987 << -46;
  ASSERT_STREQ(s.data(), "42-7895456236987-46");
}

TEST(Stringifier, long) {
  // First insertion.
  misc::stringifier s;
  s << 42l;
  ASSERT_STREQ(s.data(), "42");

  // Second insertions.
  s << -7895456l << 236987l << -46l;
  ASSERT_STREQ(s.data(), "42-7895456236987-46");
}

TEST(Stringifier, int64_t) {
  // First insertion.
  misc::stringifier s;
  s << 741258963123ll;
  ASSERT_STREQ(s.data(), "741258963123");

  // Second insertions.
  s << -12345675989ll << 3214569874110ll;
  ASSERT_STREQ(s.data(), "741258963123-123456759893214569874110");
}

TEST(Stringifier, mixed) {
  misc::stringifier s;
  s << 147852l << 894567891023ull << "foobar" << -425789751234547ll
    << std::string("baz   qux") << false << -42 << "2156" << true;

  // Check resulting string.
  ASSERT_STREQ(s.data(),
               "147852894567891023foobar-425789751234547b"
               "az   quxfalse-422156true");
}

TEST(Stringifier, pointer) {
  // Insertion.
  misc::stringifier s;
  s << &s;

  // Check content.
  ASSERT_FALSE(&s != (void*)strtoull(s.data(), NULL, 0));
}

TEST(Stringifier, reset) {
  // Insert data.
  misc::stringifier s;
  s << 42 << "foo" << -789456;
  ASSERT_STREQ(s.data(), "42foo-789456");

  // Reset buffer.
  s.reset();
  ASSERT_STREQ(s.data(), "");

  // Insert new data.
  s << "baz qux" << true << -741236;
  ASSERT_STREQ(s.data(), "baz quxtrue-741236");
}

TEST(Stringifier, string) {
  // First insertion.
  misc::stringifier s;
  s << "foo";
  ASSERT_STREQ(s.data(), "foo");

  // Second insertions.
  s << " bar"
    << " baz"
    << " qux";
  ASSERT_STREQ(s.data(), "foo bar baz qux");

  // Third insertion.
  s << static_cast<char const*>(NULL);
  ASSERT_STREQ(s.data(), "foo bar baz qux(null)");
}

TEST(Stringifier, uint) {
  // First insertion.
  misc::stringifier s;
  s << 42u;
  ASSERT_STREQ(s.data(), "42");

  // Second insertions.
  s << 7895456u << 4123456789u << 321u;
  ASSERT_STREQ(s.data(), "4278954564123456789321");
}

TEST(Stringifier, ulong) {
  // First insertion.
  misc::stringifier s;
  s << 42ul;
  ASSERT_STREQ(s.data(), "42");

  // Second insertions.
  s << 7895456ul << 4123456789ul << 321ul;
  ASSERT_STREQ(s.data(), "4278954564123456789321");
}

TEST(Stringifier, uint64_t) {
  // First insertion.
  misc::stringifier s;
  s << 71089456123456ull;
  ASSERT_STREQ(s.data(), "71089456123456");

  // Second insertions.
  s << 123456ull << 14785236987412563ull;
  ASSERT_STREQ(s.data(), "7108945612345612345614785236987412563");

  // Return test result.
}