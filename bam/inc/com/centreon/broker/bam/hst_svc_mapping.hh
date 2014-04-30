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

#ifndef CCB_BAM_HST_SVC_MAPPING_HH
#  define CCB_BAM_HST_SVC_MAPPING_HH

#  include <map>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            bam {
  /**
   *  @class hst_svc_mapping hst_svc_mapping.hh "com/centreon/broker/bam/hst_svc_mapping.hh"
   *  @brief Link name to ID.
   *
   *  Allow to find an ID of a host or service by its name.
   */
  class              hst_svc_mapping {
  public:
                     hst_svc_mapping();
                     hst_svc_mapping(hst_svc_mapping const& other);
                     ~hst_svc_mapping();
    hst_svc_mapping& operator=(hst_svc_mapping const& other);
    unsigned int     get_host_id(std::string const& name) const;
    unsigned int     get_service_id(std::string const& name) const;
    void             set_host(
                       std::string const& name,
                       unsigned int id);
    void             set_service(
                       std::string const& name,
                       unsigned int id);

  private:
    void             _internal_copy(hst_svc_mapping const& other);

    std::map<std::string, unsigned int>
                     _hosts;
    std::map<std::string, unsigned int>
                     _services;
  };
}

CCB_END()

#endif // !CCB_BAM_HST_SVC_MAPPING_HH
