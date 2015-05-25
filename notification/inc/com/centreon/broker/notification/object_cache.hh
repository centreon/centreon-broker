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

#ifndef CCB_NOTIFICATION_OBJECT_CACHE_HH
#  define CCB_NOTIFICATION_OBJECT_CACHE_HH

#  include <string>
#  include <deque>
#  include <map>
#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/logging/logging.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/neb/custom_variable_status.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class object_cache object_cache.hh "com/centreon/broker/notification/object_cache.hh"
   *  @tparam T  The type of the object cached.
   *  @brief Contain a fixed size cache of a particular object.
   */
  template <typename FullType, typename StatusType>
  class               object_cache {
  public:
    /**
      *  Default constructor
      */
                      object_cache() {}
    /**
     *  Copy constructor
     * @param[in] obj  The object to copy.
     */
                      object_cache(
                        object_cache<FullType, StatusType> const& obj) {
      object_cache<FullType, StatusType>::operator=(obj);
    }

    /**
     *  Assignment operator.
     *
     *  @param[in] obj  The obect to copy.
     *
     *  @return  A reference this object.
     */
    object_cache<FullType, StatusType>&  operator=(
      object_cache<FullType, StatusType> const& obj) {
      if (this != &obj) {
        _node = obj._node;
        _current_status = obj._current_status;
        _prev_status = obj._prev_status;
        _custom_variables = obj._custom_variables;
      }
      return (*this);
    }

    /**
     *  Serialize the data of the object cache into a vector.
     *
     *  @param[in] out  The vector to fill.
     */
    void serialize(std::deque<misc::shared_ptr<io::data> >& out) const {
      out.push_back(misc::shared_ptr<io::data>(new FullType(_node)));
      out.push_back(misc::shared_ptr<io::data>(
                      new StatusType(_prev_status)));
      out.push_back(misc::shared_ptr<io::data>(
                      new StatusType(_current_status)));
      for (QHash<std::string, neb::custom_variable_status>::const_iterator
             it(_custom_variables.begin()),
             end(_custom_variables.end());
           it != end;
           ++it)
        out.push_back(
              misc::shared_ptr<io::data>(new neb::custom_variable_status(*it)));
    }

    /**
     *  Update the object cache.
     *
     *  @param[in] n  The data to update.
     */
    void update(FullType const& n) {
      _node = n;
    }

    /**
     *  Update the object cache.
     *
     *  @param[in] status  The data to update.
     */
    void update(StatusType const& status) {
      _prev_status = _current_status;
      _current_status = status;
    }

    /**
     *  Update the object cache.
     *
     *  @param[in] var  The data to update.
     */
    void update(neb::custom_variable_status const& var) {
      std::string var_name;
      var_name = (var.service_id ? "_SERVICE" : "_HOST");
      var_name.append(var.name.toStdString());
      if (var.value.isEmpty()) {
        logging::debug(logging::low)
          << "notification: removing custom variable '"
          << var_name << "' from node (" << var.host_id
          << ", " << var.service_id << ")";
        _custom_variables.remove(var_name);
      }
      else {
        logging::debug(logging::low)
          << "notification: adding custom variable '"
          << var_name << "' to node (" << var.host_id
          << ", " << var.service_id << ")";
        _custom_variables.insert(var_name, var);
      }
    }


    /**
     *  Get the node object.
     *
     *  @return  The node object.
     */
    FullType const& get_node() const {
      return (_node);
    }

    /**
     *  Get the status object.
     *
     *  @return  The status object.
     */
    StatusType const& get_status() const {
      return (_current_status);
    }

    /**
     *  Get the prev status.
     *
     *  @return  The prev status.
     */
    StatusType const& get_prev_status() const {
      return (_prev_status);
    }

    /**
     *  Get the custom vars of this node.
     *
     *  @return  The custome vars of this node.
     */
    QHash<std::string, neb::custom_variable_status> const& get_custom_vars() const {
      return (_custom_variables);
    }

  private:
    FullType    _node;
    StatusType  _current_status;
    StatusType  _prev_status;

    QHash<std::string, neb::custom_variable_status>
                _custom_variables;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
