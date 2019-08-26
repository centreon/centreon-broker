/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_OBJECT_CACHE_HH
#  define CCB_NOTIFICATION_OBJECT_CACHE_HH

#  include <deque>
#  include <memory>
#  include <string>
#  include <unordered_map>
#  include "com/centreon/broker/namespace.hh"
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
    void serialize(std::deque<std::shared_ptr<io::data> >& out) const {
      out.push_back(std::make_shared<FullType>(_node));
      out.push_back(std::make_shared<StatusType>(_prev_status));
      out.push_back(std::make_shared<StatusType>(_current_status));
      for (std::unordered_map<std::string, neb::custom_variable_status>::const_iterator
             it{_custom_variables.begin()},
             end{_custom_variables.end()};
           it != end;
           ++it)
        out.push_back(std::make_shared<neb::custom_variable_status>(it->second));
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
      std::string var_name{var.service_id ? "_SERVICE" : "_HOST"};
      var_name.append(var.name);
      if (var.value.empty()) {
        logging::debug(logging::low)
          << "notification: removing custom variable '"
          << var_name << "' from node (" << var.host_id
          << ", " << var.service_id << ")";
        _custom_variables.erase(var_name);
      }
      else {
        logging::debug(logging::low)
          << "notification: adding custom variable '"
          << var_name << "' to node (" << var.host_id
          << ", " << var.service_id << ")";
        _custom_variables.insert({var_name, var});
      }
    }


    /**
     *  Get the node object.
     *
     *  @return  The node object.
     */
    FullType const& get_node() const {
      return _node;
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
    std::unordered_map<std::string, neb::custom_variable_status> const&
    get_custom_vars() const {
      return _custom_variables;
    }

  private:
    FullType    _node;
    StatusType  _current_status;
    StatusType  _prev_status;

    std::unordered_map<std::string, neb::custom_variable_status>
        _custom_variables;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
