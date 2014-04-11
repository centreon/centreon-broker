/*
** Copyright 2014 Merethis
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
#ifndef CC_COMMON_TOOLS_CREATE_MAP_HH
#  define CC_COMMON_TOOLS_CREATE_MAP_HH
#  include <map>

namespace com{
  namespace centreon{
    namespace tools{
      /**
       *  @class  create_map
       *  @brief  A clever piece of code found on the net to
       *          automate the loading of a map from a literal expression.
       */
      template <typename T, typename U>
      class create_map {
      public:
        /**
         *  Constructor
         */
        create_map(T const& key,U const& val) {
          m_map[key] = val;
        }

        /**
         *  @brief Operator (key, value).
         *
         *  This operator takes the same parameters as constructor so that the
         *  row for the constructor and all preceding rows are identical. This
         *  allows for a clean layout of table rows.
         *
         *  @param[in] key Key.
         *  @param[in] val Value.
         *
         *  @return This object.
         */
        create_map<T,U>& operator()(T const& key, U const& val) {
          m_map[key] = val;
          return (*this);
        }

        /**
         *  Operator map
         *
         *  @return  Returns the internal map loaded with all the values of
         *           the literal table.
         */
        operator std::map<T, U>() {
          return (m_map);
        }

      private:
        std::map<T,U> m_map;
      };

    }
  }
}
#endif
