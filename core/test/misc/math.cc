/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
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
#include "com/centreon/broker/misc/math.hh"
#include <gtest/gtest.h>

using namespace com::centreon::broker::misc;

TEST(LeastSquares, TwoPoints) {
  std::array<std::pair<int32_t, int32_t>, 5> pts{{{0, 0}, {3, 6}}};
  double a, b;
  ASSERT_TRUE(least_squares(pts, 2, a, b));
  ASSERT_EQ(a, 2);
  ASSERT_EQ(b, 0);
}

TEST(LeastSquares, BadPoints) {
  std::array<std::pair<int32_t, int32_t>, 5> pts{{{0, 0}, {0, 6}}};
  double a, b;
  ASSERT_FALSE(least_squares(pts, 2, a, b));
}

TEST(LeastSquares, OnePoint) {
  std::array<std::pair<int32_t, int32_t>, 5> pts{{{5, 6}}};
  double a, b;
  ASSERT_FALSE(least_squares(pts, 1, a, b));
}

TEST(LeastSquares, FivePoints) {
  std::array<std::pair<int32_t, int32_t>, 5> pts{
      {{-3, -5}, {1, 3}, {3, 6}, {5, 10}, {7, 14}}};
  double a, b;
  ASSERT_TRUE(least_squares(pts, 5, a, b));
  ASSERT_NEAR(a, 1.878378, 1E-5);
  ASSERT_NEAR(b, 0.7162162, 1E-5);
}
