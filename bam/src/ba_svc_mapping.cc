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

#include "com/centreon/broker/bam/ba_svc_mapping.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
ba_svc_mapping::ba_svc_mapping() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_svc_mapping::ba_svc_mapping(ba_svc_mapping const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba_svc_mapping::~ba_svc_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba_svc_mapping& ba_svc_mapping::operator=(ba_svc_mapping const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Find a host name and service description of a BA by its ID.
 *
 *  @return A pair of host name/service description.
 */
std::pair<std::string, std::string> ba_svc_mapping::get_service(
                                                      unsigned int ba_id) {
  std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator
    it(_mapping.find(ba_id));
  return ((it != _mapping.end())
          ? it->second
          : std::make_pair(std::string(), std::string()));
}

/**
 *  Link a BA ID to a pair of host name/service description.
 *
 *  @param[in] ba_id  BA ID.
 *  @param[in] hst    Host ID.
 *  @param[in] svc    Service ID.
 */
void ba_svc_mapping::set(
                       unsigned int ba_id,
                       std::string const& hst,
                       std::string const& svc) {
  _mapping[ba_id] = std::make_pair(hst, svc);
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void ba_svc_mapping::_internal_copy(ba_svc_mapping const& other) {
  _mapping = other._mapping;
  return ;
}
