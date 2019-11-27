/*
** Copyright 2019 Centreon
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

#ifndef CCB_MISC_MATH_HH
#define CCB_MISC_MATH_HH

#include <algorithm>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
/**
 *  Compute a and b such that y=ax+b is a linear regression of the pts array
 *  given in parameter. The method used is least squares.
 *
 * @param pts An array of pairs (x, y)
 * @param size If shorter than the array size, only the first size pairs of
 *             the array are taken for the regression regression.
 * @param a The computed value for the linear coefficient a (output)
 * @param b The computed  value for the constant term b (output)
 *
 * @return true if no error are seen, false otherwise.
 */
template <typename T, typename U, size_t N>
bool least_squares(std::array<std::pair<T, U>, N> const& pts,
                   size_t size,
                   double& a,
                   double& b) noexcept {
  double sx = 0;
  double sy = 0;
  double sx2 = 0;
  double sxy = 0;
  size_t count = std::min(size, static_cast<size_t>(pts.size()));

  if (count < 2)
    return false;

  for (size_t nb = 0; nb < count; ++nb) {
    sx += static_cast<double>(pts[nb].first);
    sy += static_cast<double>(pts[nb].second);
    sx2 +=
        static_cast<double>(pts[nb].first) * static_cast<double>(pts[nb].first);
    sxy += static_cast<double>(pts[nb].first) *
           static_cast<double>(pts[nb].second);
  }
  double mx = sx / count;
  double my = sy / count;
  double cov_xy = sxy / count - mx * my;
  double sd_x2 = sx2 / count - mx * mx;

  if (sd_x2)
    a = cov_xy / sd_x2;
  else
    return false;
  b = my - a * mx;

  return true;
}
}

CCB_END()

#endif  // !CCB_MISC_MATH_HH
