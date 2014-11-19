/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH

#  include <vector>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class composed_builder composed_builder.hh "com/centreon/broker/notification/builders/composed_builder.hh"
   *  @tparam T The builder type for the underlying builders.
   *  @brief Base for composed builders.
   *
   *  This class provides facilities for composed builders.
   */
  template <typename T>
  class           composed_builder : public T {
  public:
    typedef std::vector<T*> vector_type;
    typedef typename vector_type::iterator iterator;
    typedef typename vector_type::const_iterator const_iterator;

                  composed_builder() {}
    virtual       ~composed_builder() {}

    void          push_back(T& builder) {
      _builders.push_back(&builder);
    }

    iterator      begin() {
      return (_builders.begin());
    }

    const_iterator begin() const {
      return (_builders.begin());
    }

    iterator       end() {
      return (_builders.end());
    }

    const_iterator end() const {
      return (_builders.end());
    }

  private:
    std::vector<T*> _builders;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH
