/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/mapping/property.hh"
#include "com/centreon/broker/mapping/source.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    mapping {
  /**
   *  @class entry entry.hh "com/centreon/broker/mapping/entry.hh"
   *  @brief Mapping class.
   *
   *  Holds a member with a name and a data source.
   */
  class                      entry {
  private:
    QString                  _name;
    source*                  _ptr;
    misc::shared_ptr<source> _source;
    source::source_type      _type;

  public:
    /**
     *  @brief Constructor.
     *
     *  Build an entry from a property.
     *
     *  @param[in] name Entry name.
     *  @param[in] prop Property.
     */
    template <typename T, typename U>
                             entry(
                               U (T::* prop),
                               QString const& name) {
      _name = name;
      _source = misc::shared_ptr<source>(new property<T>(prop, &_type));
      _ptr = _source.data();
    }

                             entry();
                             entry(entry const& e);
                             ~entry();
    entry&                   operator=(entry const& e);
    bool                     get_bool(io::data const& d);
    double                   get_double(io::data const& d);
    int                      get_int(io::data const& d);
    short                    get_short(io::data const& d);
    QString const&           get_string(io::data const& d);
    time_t                   get_time(io::data const& d);
    unsigned int             get_uint(io::data const& d);
    unsigned short           get_ushort(io::data const& d);
    void                     set_bool(io::data& d, bool value);
    void                     set_double(io::data& d, double value);
    void                     set_int(io::data& d, int value);
    void                     set_short(io::data& d, short value);
    void                     set_string(
                               io::data& d,
                               QString const& value);
    void                     set_time(io::data& d, time_t value);
    void                     set_uint(io::data& d, unsigned int value);
    void                     set_ushort(
                               io::data& d,
                               unsigned short value);
  };
}

CCB_END()
