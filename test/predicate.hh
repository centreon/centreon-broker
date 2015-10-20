/*
** Copyright 2015 Centreon
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

#ifndef CCB_TEST_PREDICATE_HH
#  define CCB_TEST_PREDICATE_HH

#  include <ctime>
#  include <QVariant>
#  include "com/centreon/broker/misc/stringifier.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       test {
  /**
   *  Class used to match some values to some predicate (another value,
   *  a range, a regex, ...).
   */
  class         predicate {
  public:
    enum        value_type {
      type_invalid = -1,
      type_null,
      type_bool,
      type_double,
      type_timet,
      type_uint
    };
    union                uval {
      bool               bval;
      double             dval;
      time_t             tval;
      unsigned int       uival;
    };


                predicate();
                predicate(value_type t);
                predicate(bool val);
                predicate(double val);
                predicate(time_t val);
                predicate(unsigned int val);
                predicate(time_t val1, time_t val2);
                predicate(predicate const& other);
                ~predicate();
    predicate&  operator=(predicate const& other);
    bool        operator==(predicate const& other) const;
    bool        operator==(QVariant const& other) const;
    bool        operator!=(predicate const& other) const;
    bool        operator!=(QVariant const& other) const;
    uval const& get_value() const;
    uval const& get_value2() const;
    value_type  get_value_type() const;
    bool        is_null() const;
    bool        is_range() const;
    bool        is_valid() const;

  private:
    bool        _range;
    value_type  _type;
    uval        _val1;
    uval        _val2;
  };
}

CCB_END()

com::centreon::broker::misc::stringifier& operator<<(
                                            com::centreon::broker::misc::stringifier& s,
                                            com::centreon::broker::test::predicate const& p);

#endif // !CCB_TEST_PREDICATE_HH
