/*
** Copyright 2011-2012,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_MAPPING_ENTRY_HH
#  define CCB_MAPPING_ENTRY_HH

#  include "com/centreon/broker/mapping/property.hh"
#  include "com/centreon/broker/mapping/source.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                    mapping {
  /**
   *  @class entry entry.hh "com/centreon/broker/mapping/entry.hh"
   *  @brief Mapping class.
   *
   *  Holds a member with a name and a data source.
   */
  class                      entry {
  public:
    enum                     attribute {
      NULL_ON_NOTHING = 0,
      NULL_ON_ZERO,
      NULL_ON_MINUS_ONE
    };

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
                               QString const& name,
                               unsigned int number,
                               attribute attr = NULL_ON_NOTHING) {
      _name = name;
      _source = misc::shared_ptr<source>(new property<T>(prop, &_type));
      _ptr = _source.data();
      _number = number;
      _attribute = attr;
    }

                             entry();
                             entry(entry const& other);
                             ~entry();
    entry&                   operator=(entry const& other);
    attribute                get_attribute() const;
    bool                     get_bool(io::data const& d) const;
    double                   get_double(io::data const& d) const;
    int                      get_int(io::data const& d) const;
    unsigned int             get_number() const;
    short                    get_short(io::data const& d) const;
    QString const&           get_string(io::data const& d) const;
    timestamp const&         get_time(io::data const& d) const;
    unsigned int             get_type() const;
    unsigned int             get_uint(io::data const& d) const;
    unsigned short           get_ushort(io::data const& d) const;
    void                     set_bool(io::data& d, bool value) const;
    void                     set_double(
                               io::data& d,
                               double value) const;
    void                     set_int(io::data& d, int value) const;
    void                     set_short(io::data& d, short value) const;
    void                     set_string(
                               io::data& d,
                               QString const& value) const;
    void                     set_time(
                               io::data& d,
                               timestamp const& value) const;
    void                     set_uint(
                               io::data& d,
                               unsigned int value) const;
    void                     set_ushort(
                               io::data& d,
                               unsigned short value) const;
    bool                     is_null() const;

  private:
    attribute                _attribute;
    QString                  _name;
    unsigned int             _number;
    source*                  _ptr;
    misc::shared_ptr<source> _source;
    source::source_type      _type;
  };
}

CCB_END()

#endif // !CCB_MAPPING_ENTRY_HH
