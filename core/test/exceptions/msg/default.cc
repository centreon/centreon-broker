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
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

// Given an exceptions::msg object
// When it is default constructed
// Then its error message is empty
TEST(ExceptionsMsgDefault, DefaultConstructor) {
  exceptions::msg e;
  ASSERT_STREQ(e.what(), "");
}

// Given an exceptions::msg object with an error message
// When a copy of this object is created through the copy constructor
// Then the error messages of the two objects are equal
TEST(ExceptionsMsgDefault, CopyConstructor) {
  exceptions::msg e1;
  e1 << "This is my test error message (" << 42 << ") !";
  exceptions::msg e2(e1);
  ASSERT_STREQ(e1.what(), e2.what());
}

// Given an exceptions::msg object with an error message
// And another exceptions::msg object with another error message
// When the first object is assigned the second
// Then the error messages of the two objects are the second error message
TEST(ExceptionsMsgDefault, AssignmentOperator) {
  char const* m1("This is the first error message !");
  exceptions::msg e1;
  e1 << m1;
  char const* m2("This is the second error messages !");
  exceptions::msg e2;
  e2 << m2;
  e1 = e2;
  ASSERT_STREQ(e1.what(), m2);
  ASSERT_STREQ(e2.what(), m2);
}
