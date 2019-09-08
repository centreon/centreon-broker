/*
** Copyright 2013-2014 Centreon
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

#ifndef CCB_IO_PROPERTIES_HH
#define CCB_IO_PROPERTIES_HH

#include <list>
#include <map>
#include <string>
#include "com/centreon/broker/io/property.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class properties properties.hh "com/centreon/broker/io/properties.hh"
 *  @brief Properties of endpoints.
 *
 *  This class is used to gather information about endpoints.
 */
class properties {
 public:
  typedef std::map<std::string, property>::iterator iterator;
  typedef std::map<std::string, property>::const_iterator const_iterator;
  typedef std::list<std::pair<std::string, properties> > children_list;

  properties(std::string const& name = "");
  properties(properties const& right);
  ~properties() throw();
  properties& operator=(properties const& right);
  property const& operator[](std::string const& name) const;
  property& operator[](std::string const& name);
  iterator begin();
  const_iterator begin() const;
  children_list const& children() const;
  children_list& children();
  void add_child(io::properties const& child,
                 std::string const& name = std::string());
  iterator end();
  const_iterator end() const;
  property const& get(std::string const& name) const;
  void merge(properties const& other);
  std::string const& name() const throw();
  void name(std::string const& name);
  void add_property(std::string const& name, property const& prop);

 private:
  children_list _children;
  std::string _name;
  std::map<std::string, property> _properties;
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_PROPERTIES_HH
