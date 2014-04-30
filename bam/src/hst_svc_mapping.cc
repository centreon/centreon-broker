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

#include "com/centreon/broker/bam/hst_svc_mapping.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
hst_svc_mapping::hst_svc_mapping() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
hst_svc_mapping::hst_svc_mapping(hst_svc_mapping const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
hst_svc_mapping::~hst_svc_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
hst_svc_mapping& hst_svc_mapping::operator=(
                                    hst_svc_mapping const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Get host ID by its name.
 *
 *  @param[in] name  Host name.
 *
 *  @return Host ID, 0 if it was not found.
 */
unsigned int hst_svc_mapping::get_host_id(
                                std::string const& name) const {
  std::map<std::string, unsigned int>::const_iterator
    it(_hosts.find(name));
  return ((it != _hosts.end()) ? it->second : 0);
}

/**
 *  Get service ID by its name.
 *
 *  @param[in] name  Service description.
 *
 *  @return Service ID, 0 if it was not found.
 */
unsigned int hst_svc_mapping::get_service_id(
                                std::string const& name) const {
  std::map<std::string, unsigned int>::const_iterator
    it(_services.find(name));
  return ((it != _services.end()) ? it->second : 0);
}

/**
 *  Set the ID of a host.
 *
 *  @param[in] name  Host name.
 *  @param[in] id    Host ID.
 */
void hst_svc_mapping::set_host(
                        std::string const& name,
                        unsigned int id) {
  _hosts[name] = id;
  return ;
}

/**
 *  Set the ID of a service.
 *
 *  @param[in] name  Service description.
 *  @param[in] id    Service ID.
 */
void hst_svc_mapping::set_service(
                        std::string const& name,
                        unsigned int id) {
  _services[name] = id;
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void hst_svc_mapping::_internal_copy(hst_svc_mapping const& other) {
  _hosts = other._hosts;
  _services = other._services;
  return ;
}
