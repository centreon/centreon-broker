/*
** Copyright 2014 Centreon
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
 *  @param[in] hst  Host name.
 *
 *  @return Host ID, 0 if it was not found.
 */
unsigned int hst_svc_mapping::get_host_id(
                                std::string const& hst) const {
  return (get_service_id(hst, "").first);
}

/**
 *  Get service ID by its name.
 *
 *  @param[in] hst  Host name.
 *  @param[in] svc  Service description.
 *
 *  @return Pair of integers with host ID and service ID, (0, 0) if it
 *          was not found.
 */
std::pair<unsigned int, unsigned int> hst_svc_mapping::get_service_id(
                                                         std::string const& hst,
                                                         std::string const& svc) const {
  std::map<std::pair<std::string, std::string>,
           std::pair<unsigned int, unsigned int> >::const_iterator
    it(_mapping.find(std::make_pair(hst, svc)));
  return ((it != _mapping.end()) ? it->second : std::make_pair(0u, 0u));
}

/**
 *  Set the ID of a host.
 *
 *  @param[in] hst      Host name.
 *  @param[in] host_id  Host ID.
 */
void hst_svc_mapping::set_host(
                        std::string const& hst,
                        unsigned int host_id) {
  set_service(hst, "", host_id, 0u, true);
  return ;
}

/**
 *  Set the ID of a service.
 *
 *  @param[in] hst         Host name.
 *  @param[in] svc         Service description.
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 */
void hst_svc_mapping::set_service(
                        std::string const& hst,
                        std::string const& svc,
                        unsigned int host_id,
                        unsigned int service_id,
                        bool activated) {
  _mapping[std::make_pair(hst, svc)] = std::make_pair(host_id, service_id);
  _activated_mapping[std::make_pair(host_id, service_id)] = activated;
  return ;
}

/**
 *  Get if the service is activated.
 *
 *  @param[in] hst_id       The host id.
 *  @param[in] service_id   The service id.
 *
 *  @return                 True if activated.
 */
bool hst_svc_mapping::get_activated (
                        unsigned int hst_id,
                        unsigned int service_id) const {
  std::map<std::pair<unsigned int, unsigned int>, bool>::const_iterator
        it (_activated_mapping.find(std::make_pair(hst_id, service_id)));
  return (it == _activated_mapping.end() ? true : it->second);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void hst_svc_mapping::_internal_copy(hst_svc_mapping const& other) {
  _mapping = other._mapping;
  return ;
}
